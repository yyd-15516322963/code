#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <string.h>
#include <time.h>

// 硬件参数配置
#define DISP_WIDTH     800     // 显示屏分辨率
#define DISP_HEIGHT    480
#define TOUCH_WIDTH    1024    // 触摸屏分辨率
#define TOUCH_HEIGHT   600

// 游戏参数配置 (优化尺寸适配)
#define GRID_SIZE      4
#define CELL_SIZE      90      // 单元格大小
#define CELL_PADDING   8       // 单元格间距
#define GAME_OFFSET_X  40      // 游戏区域X偏移
#define GAME_OFFSET_Y  40      // 游戏区域Y偏移

// 分数面板配置 (右侧独立区域)
#define SCORE_PANEL_X  550     // 分数面板X起始
#define SCORE_PANEL_Y  80      // 分数面板Y起始
#define SCORE_PANEL_W  200     // 分数面板宽度
#define SCORE_PANEL_H  100     // 分数面板高度

// 颜色定义 (ARGB8888 格式)
#define COLOR_BG_MAIN     0xFFF5F5F5  // 主背景色
#define COLOR_BG_GRID     0xFFBBADA0  // 棋盘背景色
#define COLOR_BG_SCORE    0xFF8F7A66  // 得分面板背景
#define COLOR_TEXT_SCORE  0xFFFFFFFF  // 得分文字色
#define COLOR_TEXT_LIGHT  0xFF776E65  // 浅色数字文字
#define COLOR_TEXT_DARK   0xFFFFFFFF  // 深色数字文字

// 数字方块颜色表 (索引对应2的幂次: 0=空,1=2,2=4...12=2048)
static const unsigned int cell_color_map[] = {
    0xFFCDC1B4, // 0 (空)
    0xFFEEE4DA, // 2
    0xFFEDE0C8, // 4
    0xFFF2B179, // 8
    0xFFF59563, // 16
    0xFFF67C5F, // 32
    0xFFF65E3B, // 64
    0xFFEDCF72, // 128
    0xFFEDCC61, // 256
    0xFFEDC850, // 512
    0xFFEDC53F, // 1024
    0xFFEDC22E, // 2048
    0xFF3C3A32  // 更高数字
};

// 游戏数据结构
typedef struct {
    int grid[GRID_SIZE][GRID_SIZE];  // 游戏网格
    int score;                       // 当前得分
    int game_over;                   // 游戏结束标志
} Game2048;

// 帧缓冲设备结构体 (内存映射核心)
typedef struct {
    int fd;                          // 设备文件描述符
    unsigned int *mem;               // 映射内存指针
    struct fb_var_screeninfo var;    // 可变屏幕参数
    struct fb_fix_screeninfo fix;    // 固定屏幕参数
    int bytes_per_pixel;             // 每个像素的字节数
    int screen_size;                 // 屏幕总字节数
} FbDevice;

// 全局设备对象
static FbDevice fb_dev;      // 帧缓冲设备
static int touch_fd = -1;    // 触摸屏设备

// ==================== 内存映射核心函数 ====================
/**
 * 初始化帧缓冲设备 (标准内存映射实现)
 */
int fb_device_init(const char *fb_path) {
    // 1. 打开帧缓冲设备
    fb_dev.fd = open(fb_path, O_RDWR);
    if (fb_dev.fd < 0) {
        perror("Failed to open framebuffer device");
        return -1;
    }

    // 2. 获取固定屏幕参数
    if (ioctl(fb_dev.fd, FBIOGET_FSCREENINFO, &fb_dev.fix) < 0) {
        perror("Failed to get fixed screen info");
        close(fb_dev.fd);
        return -1;
    }

    // 3. 获取可变屏幕参数并配置为800x480
    if (ioctl(fb_dev.fd, FBIOGET_VSCREENINFO, &fb_dev.var) < 0) {
        perror("Failed to get variable screen info");
        close(fb_dev.fd);
        return -1;
    }

    // 保存原始参数，用于退出时恢复
    struct fb_var_screeninfo orig_var = fb_dev.var;
    
    // 配置屏幕参数
    fb_dev.var.xres = DISP_WIDTH;
    fb_dev.var.yres = DISP_HEIGHT;
    fb_dev.var.xres_virtual = DISP_WIDTH;
    fb_dev.var.yres_virtual = DISP_HEIGHT;
    fb_dev.var.bits_per_pixel = 32;  // ARGB8888
    fb_dev.var.red.offset = 16;
    fb_dev.var.red.length = 8;
    fb_dev.var.green.offset = 8;
    fb_dev.var.green.length = 8;
    fb_dev.var.blue.offset = 0;
    fb_dev.var.blue.length = 8;
    fb_dev.var.transp.offset = 24;
    fb_dev.var.transp.length = 8;

    // 应用参数配置
    if (ioctl(fb_dev.fd, FBIOPUT_VSCREENINFO, &fb_dev.var) < 0) {
        perror("Failed to set variable screen info");
        close(fb_dev.fd);
        return -1;
    }

    // 4. 重新获取配置后的参数
    if (ioctl(fb_dev.fd, FBIOGET_VSCREENINFO, &fb_dev.var) < 0) {
        perror("Failed to get updated variable info");
        close(fb_dev.fd);
        return -1;
    }

    // 5. 计算内存映射参数
    fb_dev.bytes_per_pixel = fb_dev.var.bits_per_pixel / 8;
    fb_dev.screen_size = fb_dev.var.xres * fb_dev.var.yres * fb_dev.bytes_per_pixel;

    // 6. 执行内存映射 (核心步骤)
    fb_dev.mem = (unsigned int *)mmap(
        NULL,                   // 让系统选择映射地址
        fb_dev.screen_size,     // 映射内存大小
        PROT_READ | PROT_WRITE, // 读写权限
        MAP_SHARED,             // 共享映射
        fb_dev.fd,              // 帧缓冲文件描述符
        0                       // 偏移量
    );

    // 检查映射结果
    if (fb_dev.mem == MAP_FAILED) {
        perror("Failed to mmap framebuffer memory");
        ioctl(fb_dev.fd, FBIOPUT_VSCREENINFO, &orig_var); // 恢复原始参数
        close(fb_dev.fd);
        return -1;
    }

    printf("Framebuffer initialized successfully:\n");
    printf("  Resolution: %dx%d\n", fb_dev.var.xres, fb_dev.var.yres);
    printf("  Bits per pixel: %d\n", fb_dev.var.bits_per_pixel);
    printf("  Mapped memory size: %d bytes\n", fb_dev.screen_size);
    printf("  Mapped address: %p\n", fb_dev.mem);

    return 0;
}

/**
 * 释放帧缓冲设备资源
 */
void fb_device_deinit(void) {
    if (fb_dev.mem != MAP_FAILED) {
        munmap(fb_dev.mem, fb_dev.screen_size); // 解除内存映射
        fb_dev.mem = NULL;
    }
    if (fb_dev.fd >= 0) {
        close(fb_dev.fd);
        fb_dev.fd = -1;
    }
}

/**
 * 绘制单个像素 (直接操作映射内存)
 */
static inline void draw_pixel(int x, int y, unsigned int color) {
    // 边界检查
    if (x < 0 || x >= DISP_WIDTH || y < 0 || y >= DISP_HEIGHT) {
        return;
    }
    // 直接通过内存偏移访问像素 (核心优化)
    fb_dev.mem[y * DISP_WIDTH + x] = color;
}

/**
 * 绘制填充矩形 (批量内存操作)
 */
void draw_rect(int x, int y, int w, int h, unsigned int color) {
    // 边界检查
    if (x < 0 || y < 0 || w <= 0 || h <= 0 || 
        x + w > DISP_WIDTH || y + h > DISP_HEIGHT) {
        return;
    }

    // 逐行绘制 (利用内存连续性优化)
    int row, col;
    unsigned int *pixel_ptr;
    
    for (row = 0; row < h; row++) {
        // 计算当前行的起始内存地址
        pixel_ptr = &fb_dev.mem[(y + row) * DISP_WIDTH + x];
        // 填充整行像素
        for (col = 0; col < w; col++) {
            *pixel_ptr++ = color;
        }
    }
}

// ==================== 文字绘制核心优化 (彻底解决变形) ====================
/**
 * 绘制单个数字字符 (固定尺寸，无变形)
 */
void draw_digit_fixed(int x, int y, int digit, unsigned int color, int size) {
    if (digit < 0 || digit > 9) return;
    
    // 固定8x8数字点阵 (无缩放变形)
    const unsigned char num_font[10][8] = {
        {0x3e,0x61,0x51,0x49,0x45,0x63,0x3e,0x00}, // 0
        {0x00,0x21,0x7f,0x01,0x00,0x00,0x00,0x00}, // 1
        {0x27,0x45,0x49,0x49,0x49,0x49,0x31,0x00}, // 2
        {0x22,0x49,0x49,0x49,0x49,0x49,0x36,0x00}, // 3
        {0x0c,0x14,0x24,0x44,0x7f,0x04,0x04,0x00}, // 4
        {0x72,0x51,0x49,0x49,0x49,0x49,0x66,0x00}, // 5
        {0x3e,0x41,0x49,0x49,0x49,0x49,0x36,0x00}, // 6
        {0x40,0x40,0x40,0x40,0x40,0x40,0x7f,0x00}, // 7
        {0x36,0x49,0x49,0x49,0x49,0x49,0x36,0x00}, // 8
        {0x32,0x49,0x49,0x49,0x49,0x49,0x3e,0x00}  // 9
    };

    // 计算缩放比例 (保持正方形，避免变形)
    int scale = size / 8;
    if (scale < 1) scale = 1;
    
    // 绘制数字点阵
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (num_font[digit][i] & (1 << (7 - j))) {
                // 绘制正方形像素块，确保无变形
                draw_rect(x + j*scale, y + i*scale, scale, scale, color);
            }
        }
    }
}

/**
 * 绘制单个字母字符 (固定尺寸)
 */
void draw_letter_fixed(int x, int y, char letter, unsigned int color, int size) {
    // 只支持SCORE几个字母
    unsigned char font[8] = {0};
    
    switch(letter) {
        case 'S': font[0] = 0x27; font[1] = 0x45; font[2] = 0x49; font[3] = 0x49; 
                  font[4] = 0x49; font[5] = 0x45; font[6] = 0x27; font[7] = 0x00; break;
        case 'C': font[0] = 0x3e; font[1] = 0x41; font[2] = 0x41; font[3] = 0x41; 
                  font[4] = 0x41; font[5] = 0x41; font[6] = 0x3e; font[7] = 0x00; break;
        case 'O': font[0] = 0x3e; font[1] = 0x63; font[2] = 0x51; font[3] = 0x49; 
                  font[4] = 0x45; font[5] = 0x63; font[6] = 0x3e; font[7] = 0x00; break;
        case 'R': font[0] = 0x3e; font[1] = 0x47; font[2] = 0x49; font[3] = 0x49; 
                  font[4] = 0x59; font[5] = 0x69; font[6] = 0x36; font[7] = 0x00; break;
        case 'E': font[0] = 0x7f; font[1] = 0x49; font[2] = 0x49; font[3] = 0x49; 
                  font[4] = 0x49; font[5] = 0x49; font[6] = 0x49; font[7] = 0x00; break;
        default: return;
    }

    int scale = size / 8;
    if (scale < 1) scale = 1;
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (font[i] & (1 << (7 - j))) {
                draw_rect(x + j*scale, y + i*scale, scale, scale, color);
            }
        }
    }
}

/**
 * 绘制字符串 (无变形)
 */
void draw_string_fixed(int x, int y, const char *str, unsigned int color, int size) {
    int pos = 0;
    while (*str) {
        if (*str >= '0' && *str <= '9') {
            draw_digit_fixed(x + pos*(size + 4), y, *str - '0', color, size);
        } else if (*str >= 'A' && *str <= 'Z') {
            draw_letter_fixed(x + pos*(size + 4), y, *str, color, size);
        }
        pos++;
        str++;
    }
}

/**
 * 绘制分数 (右侧面板，固定尺寸，居中显示)
 */
void draw_score_panel(int score) {
    // 1. 绘制分数面板背景
    draw_rect(SCORE_PANEL_X, SCORE_PANEL_Y, SCORE_PANEL_W, SCORE_PANEL_H, COLOR_BG_SCORE);
    
    // 2. 绘制"SCORE"标题 (固定24号字体)
    draw_string_fixed(SCORE_PANEL_X + 20, SCORE_PANEL_Y + 10, "SCORE", COLOR_TEXT_SCORE, 24);
    
    // 3. 格式化分数字符串
    char score_str[20];
    sprintf(score_str, "%06d", score); // 固定6位显示
    
    // 4. 计算分数居中位置
    int score_x = SCORE_PANEL_X + (SCORE_PANEL_W - 6*(32 + 4))/2;
    
    // 5. 绘制分数 (固定32号字体，无变形)
    draw_string_fixed(score_x, SCORE_PANEL_Y + 50, score_str, COLOR_TEXT_SCORE, 32);
}

/**
 * 绘制单元格数字 (无变形)
 */
void draw_cell_number(int x, int y, int value, int cell_size) {
    if (value == 0) return;
    
    // 根据数字大小选择字体尺寸
    int font_size;
    if (value >= 1024) font_size = 16;    // 大数字用小字体
    else if (value >= 100) font_size = 20; // 中数字用中字体
    else font_size = 24;                   // 小数字用大字体
    
    // 选择文字颜色
    unsigned int text_color = (value >= 8) ? COLOR_TEXT_DARK : COLOR_TEXT_LIGHT;
    
    // 格式化数字字符串
    char num_str[10];
    sprintf(num_str, "%d", value);
    
    // 计算居中位置
    int str_len = strlen(num_str);
    int num_x = x + (cell_size - str_len*(font_size + 4))/2;
    int num_y = y + (cell_size - font_size)/2;
    
    // 绘制数字
    draw_string_fixed(num_x, num_y, num_str, text_color, font_size);
}

// ==================== 游戏功能实现 ====================
/**
 * 初始化触摸屏
 */
int touch_init(const char *touch_path) {
    touch_fd = open(touch_path, O_RDWR);
    if (touch_fd < 0) {
        perror("Failed to open touch device");
        return -1;
    }
    return 0;
}

/**
 * 触摸屏坐标转换 (1024x600 -> 800x480)
 */
void touch_convert_coords(int *x, int *y) {
    *x = (int)(((float)*x / TOUCH_WIDTH) * DISP_WIDTH);
    *y = (int)(((float)*y / TOUCH_HEIGHT) * DISP_HEIGHT);
}

/**
 * 检测滑动方向
 */
int detect_swipe(int start_x, int start_y, int end_x, int end_y) {
    int dx = end_x - start_x;
    int dy = end_y - start_y;
    int abs_dx = abs(dx);
    int abs_dy = abs(dy);

    // 滑动阈值过滤无效操作
    if (abs_dx < 30 && abs_dy < 30) {
        return 0;
    }

    // 1=上, 2=下, 3=左, 4=右
    if (abs_dx > abs_dy) {
        return dx > 0 ? 4 : 3;
    } else {
        return dy > 0 ? 2 : 1;
    }
}

/**
 * 初始化游戏
 */
void game_init(Game2048 *game) {
    // 清空网格
    memset(game->grid, 0, sizeof(game->grid));
    game->score = 0;
    game->game_over = 0;

    // 随机生成两个初始数字
    srand(time(NULL));
    int pos1 = rand() % 16;
    int pos2;
    do {
        pos2 = rand() % 16;
    } while (pos2 == pos1);

    game->grid[pos1 / GRID_SIZE][pos1 % GRID_SIZE] = 2;
    game->grid[pos2 / GRID_SIZE][pos2 % GRID_SIZE] = (rand() % 10 == 0) ? 4 : 2;
}

/**
 * 获取数字对应的颜色索引
 */
int get_color_index(int value) {
    if (value == 0) return 0;
    int index = 0;
    while (value > 2 && index < 12) {
        value /= 2;
        index++;
    }
    return index > 12 ? 12 : index;
}

/**
 * 绘制游戏界面 (最终优化版)
 */
void draw_game_ui(Game2048 *game) {
    // 1. 清空屏幕
    draw_rect(0, 0, DISP_WIDTH, DISP_HEIGHT, COLOR_BG_MAIN);

    // 2. 绘制右侧分数面板 (核心修改)
    draw_score_panel(game->score);

    // 3. 计算游戏区域总尺寸
    int game_total_width = GRID_SIZE * (CELL_SIZE + CELL_PADDING) - CELL_PADDING;
    int game_total_height = GRID_SIZE * (CELL_SIZE + CELL_PADDING) - CELL_PADDING;

    // 4. 绘制游戏棋盘背景
    draw_rect(GAME_OFFSET_X, GAME_OFFSET_Y, 
             game_total_width, game_total_height,
             COLOR_BG_GRID);

    // 5. 绘制数字方块
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int cell_x = GAME_OFFSET_X + j * (CELL_SIZE + CELL_PADDING);
            int cell_y = GAME_OFFSET_Y + i * (CELL_SIZE + CELL_PADDING);
            int value = game->grid[i][j];
            
            // 绘制方块背景
            draw_rect(cell_x, cell_y, CELL_SIZE, CELL_SIZE, 
                     cell_color_map[get_color_index(value)]);

            // 绘制数字 (无变形)
            draw_cell_number(cell_x, cell_y, value, CELL_SIZE);
        }
    }

    // 6. 游戏结束提示
    if (game->game_over) {
        // 半透明遮罩
        draw_rect(DISP_WIDTH/2 - 120, DISP_HEIGHT/2 - 60, 240, 100, 0x80000000);
        // 游戏结束文字
        draw_string_fixed(DISP_WIDTH/2 - 50, DISP_HEIGHT/2 - 30, "GAME OVER", COLOR_TEXT_DARK, 24);
        draw_string_fixed(DISP_WIDTH/2 - 70, DISP_HEIGHT/2 + 5, "FINAL SCORE", COLOR_TEXT_DARK, 16);
        
        // 显示最终分数
        char final_score[20];
        sprintf(final_score, "%06d", game->score);
        draw_string_fixed(DISP_WIDTH/2 - 48, DISP_HEIGHT/2 + 30, final_score, COLOR_TEXT_DARK, 16);
        
        draw_string_fixed(DISP_WIDTH/2 - 80, DISP_HEIGHT/2 + 55, "Touch to restart", COLOR_TEXT_DARK, 12);
    }
}

/**
 * 生成新数字
 */
int generate_new_tile(Game2048 *game) {
    // 收集空位置
    int empty[GRID_SIZE*GRID_SIZE][2];
    int count = 0;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (game->grid[i][j] == 0) {
                empty[count][0] = i;
                empty[count][1] = j;
                count++;
            }
        }
    }

    if (count == 0) return 0;

    // 随机选择位置生成数字
    int idx = rand() % count;
    game->grid[empty[idx][0]][empty[idx][1]] = (rand() % 10 == 0) ? 4 : 2;
    return 1;
}

/**
 * 向左移动
 */
int move_left(Game2048 *game) {
    int moved = 0;

    for (int i = 0; i < GRID_SIZE; i++) {
        // 压缩空格
        for (int j = 0; j < GRID_SIZE; j++) {
            if (game->grid[i][j] == 0) {
                for (int k = j + 1; k < GRID_SIZE; k++) {
                    if (game->grid[i][k] != 0) {
                        game->grid[i][j] = game->grid[i][k];
                        game->grid[i][k] = 0;
                        moved = 1;
                        break;
                    }
                }
            }
        }

        // 合并相同数字
        for (int j = 0; j < GRID_SIZE - 1; j++) {
            if (game->grid[i][j] != 0 && game->grid[i][j] == game->grid[i][j+1]) {
                game->grid[i][j] *= 2;
                game->score += game->grid[i][j];
                game->grid[i][j+1] = 0;
                moved = 1;

                // 合并后重新压缩
                for (int k = j + 1; k < GRID_SIZE - 1; k++) {
                    game->grid[i][k] = game->grid[i][k+1];
                    game->grid[i][k+1] = 0;
                }
            }
        }
    }

    return moved;
}

/**
 * 向右移动
 */
int move_right(Game2048 *game) {
    int moved = 0;

    for (int i = 0; i < GRID_SIZE; i++) {
        // 压缩空格
        for (int j = GRID_SIZE - 1; j >= 0; j--) {
            if (game->grid[i][j] == 0) {
                for (int k = j - 1; k >= 0; k--) {
                    if (game->grid[i][k] != 0) {
                        game->grid[i][j] = game->grid[i][k];
                        game->grid[i][k] = 0;
                        moved = 1;
                        break;
                    }
                }
            }
        }

        // 合并相同数字
        for (int j = GRID_SIZE - 1; j > 0; j--) {
            if (game->grid[i][j] != 0 && game->grid[i][j] == game->grid[i][j-1]) {
                game->grid[i][j] *= 2;
                game->score += game->grid[i][j];
                game->grid[i][j-1] = 0;
                moved = 1;

                // 合并后重新压缩
                for (int k = j - 1; k > 0; k--) {
                    game->grid[i][k] = game->grid[i][k-1];
                    game->grid[i][k-1] = 0;
                }
            }
        }
    }

    return moved;
}

/**
 * 向上移动
 */
int move_up(Game2048 *game) {
    int moved = 0;

    for (int j = 0; j < GRID_SIZE; j++) {
        // 压缩空格
        for (int i = 0; i < GRID_SIZE; i++) {
            if (game->grid[i][j] == 0) {
                for (int k = i + 1; k < GRID_SIZE; k++) {
                    if (game->grid[k][j] != 0) {
                        game->grid[i][j] = game->grid[k][j];
                        game->grid[k][j] = 0;
                        moved = 1;
                        break;
                    }
                }
            }
        }

        // 合并相同数字
        for (int i = 0; i < GRID_SIZE - 1; i++) {
            if (game->grid[i][j] != 0 && game->grid[i][j] == game->grid[i+1][j]) {
                game->grid[i][j] *= 2;
                game->score += game->grid[i][j];
                game->grid[i+1][j] = 0;
                moved = 1;

                // 合并后重新压缩
                for (int k = i + 1; k < GRID_SIZE - 1; k++) {
                    game->grid[k][j] = game->grid[k+1][j];
                    game->grid[k+1][j] = 0;
                }
            }
        }
    }

    return moved;
}

/**
 * 向下移动
 */
int move_down(Game2048 *game) {
    int moved = 0;

    for (int j = 0; j < GRID_SIZE; j++) {
        // 压缩空格
        for (int i = GRID_SIZE - 1; i >= 0; i--) {
            if (game->grid[i][j] == 0) {
                for (int k = i - 1; k >= 0; k--) {
                    if (game->grid[k][j] != 0) {
                        game->grid[i][j] = game->grid[k][j];
                        game->grid[k][j] = 0;
                        moved = 1;
                        break;
                    }
                }
            }
        }

        // 合并相同数字
        for (int i = GRID_SIZE - 1; i > 0; i--) {
            if (game->grid[i][j] != 0 && game->grid[i][j] == game->grid[i-1][j]) {
                game->grid[i][j] *= 2;
                game->score += game->grid[i][j];
                game->grid[i-1][j] = 0;
                moved = 1;

                // 合并后重新压缩
                for (int k = i - 1; k > 0; k--) {
                    game->grid[k][j] = game->grid[k-1][j];
                    game->grid[k-1][j] = 0;
                }
            }
        }
    }

    return moved;
}

/**
 * 检查游戏是否结束
 */
int is_game_over(Game2048 *game) {
    // 检查是否有空位
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (game->grid[i][j] == 0) {
                return 0;
            }
        }
    }

    // 检查是否有可合并的数字
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            // 检查右侧
            if (j < GRID_SIZE - 1 && game->grid[i][j] == game->grid[i][j+1]) {
                return 0;
            }
            // 检查下侧
            if (i < GRID_SIZE - 1 && game->grid[i][j] == game->grid[i+1][j]) {
                return 0;
            }
        }
    }

    return 1;
}

/**
 * 处理游戏移动
 */
void handle_movement(Game2048 *game, int direction) {
    int moved = 0;

    switch (direction) {
        case 1: moved = move_up(game);    break; // 上
        case 2: moved = move_down(game);  break; // 下
        case 3: moved = move_left(game);  break; // 左
        case 4: moved = move_right(game); break; // 右
    }

    if (moved) {
        generate_new_tile(game);
        if (is_game_over(game)) {
            game->game_over = 1;
        }
    }
}

/**
 * 游戏主循环
 */
void game_main_loop(Game2048 *game) {
    struct input_event ev;
    int touch_start_x = 0, touch_start_y = 0;
    int touch_end_x = 0, touch_end_y = 0;
    int is_touching = 0;

    while (1) {
        // 绘制游戏界面
        draw_game_ui(game);

        // 读取触摸事件
        ssize_t ret = read(touch_fd, &ev, sizeof(ev));
        if (ret != sizeof(ev)) {
            continue;
        }

        // 处理触摸事件
        if (ev.type == EV_ABS) {
            if (ev.code == ABS_X) {
                if (is_touching) {
                    touch_end_x = ev.value;
                } else {
                    touch_start_x = ev.value;
                }
            } else if (ev.code == ABS_Y) {
                if (is_touching) {
                    touch_end_y = ev.value;
                } else {
                    touch_start_y = ev.value;
                }
            }
        } else if (ev.type == EV_KEY && ev.code == BTN_TOUCH) {
            if (ev.value == 1) {
                // 触摸按下
                is_touching = 1;
            } else {
                // 触摸抬起
                is_touching = 0;

                // 转换坐标
                touch_convert_coords(&touch_start_x, &touch_start_y);
                touch_convert_coords(&touch_end_x, &touch_end_y);

                // 检测滑动方向
                int dir = detect_swipe(touch_start_x, touch_start_y, 
                                      touch_end_x, touch_end_y);

                if (game->game_over) {
                    // 重新开始游戏
                    game_init(game);
                } else if (dir != 0) {
                    // 处理移动
                    handle_movement(game, dir);
                }
            }
        }
    }
}

/**
 * 清理资源
 */
void cleanup_resources(void) {
    fb_device_deinit();
    if (touch_fd >= 0) {
        close(touch_fd);
    }
}

// ==================== 主函数 ====================
int main(int argc, char *argv[]) {
    Game2048 game;

    // 1. 初始化帧缓冲设备 (内存映射核心)
    if (fb_device_init("/dev/fb0") != 0) {
        fprintf(stderr, "Failed to initialize framebuffer\n");
        return -1;
    }

    // 2. 初始化触摸屏
    if (touch_init("/dev/input/event0") != 0) {
        fprintf(stderr, "Failed to initialize touch screen\n");
        cleanup_resources();
        return -1;
    }

    // 3. 初始化游戏
    game_init(&game);

    // 4. 启动游戏主循环
    game_main_loop(&game);

    // 5. 清理资源
    cleanup_resources();

    return 0;
}