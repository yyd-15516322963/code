#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <strings.h>

#define N 4

void get_num(int buf[N][N])
{
lb:
    int num[2] = {2, 4};
    int row = rand() % N;
    int col = rand() % N;
    int i = rand() % 2;

    if (buf[row][col] == 0)
        buf[row][col] = num[i];
    else
        goto lb;
}

void left_move(int buf[N][N])
{
    int temp, head = 0;
    for (int i = 0; i < N; i++)
    {
        head = 0;
        for (int j = 0; j < N; j++)
        {
            if (buf[i][j] != 0)
            {
                temp = buf[i][j];
                buf[i][j] = 0;
                buf[i][head] = temp;
                head++;
            }
        }
        temp = 0;
        head = 0;
        int k = 0;
        while (k < N - 1)
        {
            if (buf[i][k] != 0 && buf[i][k] == buf[i][k + 1])
            {
                temp = buf[i][k];
                buf[i][k] = 0;
                buf[i][head] = temp * 2;
                buf[i][k + 1] = 0;
                head++;
                k++;
            }
            if (buf[i][k] != 0)
                head++;
            k++;
        }
        head = 0;
        for (int j = 0; j < N; j++)
        {
            if (buf[i][j] != 0)
            {
                temp = buf[i][j];
                buf[i][j] = 0;
                buf[i][head] = temp;
                head++;
            }
        }
    }
}

void right_move(int buf[N][N])
{
    int temp, head;
    for (int i = 0; i < N; i++)
    {
        head = N - 1;
        for (int j = N - 1; j >= 0; j--)
        {
            if (buf[i][j] != 0)
            {
                temp = buf[i][j];
                buf[i][j] = 0;
                buf[i][head] = temp;
                head--;
            }
        }
        temp = 0;
        head = N - 1;
        int k = N - 1;
        while (k > 0)
        {
            if (buf[i][k] != 0 && buf[i][k] == buf[i][k - 1])
            {
                temp = buf[i][k];
                buf[i][k] = 0;
                buf[i][head] = temp * 2;
                buf[i][k - 1] = 0;
                head--;
                k--;
            }
            if (buf[i][k] != 0)
                head--;
            k--;
        }
        head = N - 1;
        for (int j = N - 1; j >= 0; j--)
        {
            if (buf[i][j] != 0)
            {
                temp = buf[i][j];
                buf[i][j] = 0;
                buf[i][head] = temp;
                head--;
            }
        }
    }
}

void up_move(int buf[N][N])
{
    int temp, head;
    for (int j = 0; j < N; j++)
    {
        head = 0;
        for (int i = 0; i < N; i++)
        {
            if (buf[i][j] != 0)
            {
                temp = buf[i][j];
                buf[i][j] = 0;
                buf[head][j] = temp;
                head++;
            }
        }
        temp = 0;
        head = 0;
        int k = 0;
        while (k < N - 1)
        {
            if (buf[k][j] != 0 && buf[k][j] == buf[k + 1][j])
            {
                temp = buf[k][j];
                buf[k][j] = 0;
                buf[head][j] = temp * 2;
                buf[k + 1][j] = 0;
                head++;
                k++;
            }
            if (buf[k][j] != 0)
                head++;
            k++;
        }
        head = 0;
        for (int i = 0; i < N; i++)
        {
            if (buf[i][j] != 0)
            {
                temp = buf[i][j];
                buf[i][j] = 0;
                buf[head][j] = temp;
                head++;
            }
        }
    }
}
void down_move(int buf[N][N])
{
    int temp, head;
    for (int j = 0; j < N; j++)
    {
        head = N - 1;
        for (int i = N - 1; i >= 0; i--)
        {
            if (buf[i][j] != 0)
            {
                temp = buf[i][j];
                buf[i][j] = 0;
                buf[head][j] = temp;
                head--;
            }
        }
        temp = 0;
        head = N - 1;
        int k = N - 1;
        while (k > 0)
        {
            if (buf[k][j] != 0 && buf[k][j] == buf[k - 1][j])
            {
                temp = buf[k][j];
                buf[k][j] = 0;
                buf[head][j] = temp * 2;
                buf[k - 1][j] = 0;
                head--;
                k--;
            }
            if (buf[k][j] != 0)
                head--;
            k--;
        }
        head = N - 1;
        for (int i = N - 1; i >= 0; i--)
        {
            if (buf[i][j] != 0)
            {
                temp = buf[i][j];
                buf[i][j] = 0;
                buf[head][j] = temp;
                head--;
            }
        }
    }
}

void show(int buf[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%4d", buf[i][j]);
        }
        printf("\n");
    }
}

int game_over(int buf[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (buf[i][j] == 0)
            {
                return 0;
            }
        }
    }

    // for (int i = 0; i < 4; i++)
    // {
    //     for (int j = 0; j < 3; j++)
    //     {
    //         if (buf[i][j] == buf[i][j + 1])
    //         {
    //             return 0;
    //         }
    //     }
    // }

    // for (int j = 0; j < 4; j++)
    // {
    //     for (int i = 0; i < 3; i++)
    //     {
    //         if (buf[i][j] == buf[i + 1][j])

    //         {
    //             return 0;
    //         }
    //     }
    // }
    return 1;
}

int game_win(int buf[N][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (buf[i][j] >= 2048)

                return 1;
        }
    }
    return 0;
}
int main(int argc, char const *argv[])
{
    srand((unsigned int)time(NULL));
    int buf[N][N] = {0};
    int i = 0;
    while (i <= (N / 2))
    {
        get_num(buf);
        i++;
    }

    char n;

    printf("============ 2048小游戏 ============\n");
    printf("操作:w(上) s(下) a(左) d(右) q(退出)\n");
    printf("====================================\n");
    show(buf);
    while (1)
    {
        if (game_over(buf) == 1)
        {
            printf("无法移动,挑战失败,游戏结束\n");
            break;
        }

        if (game_win(buf) == 1)
        {
            printf("恭喜你挑战成功！\n");
            break;
        }

        if (n == 'q')
        {
            printf("你选择退出游戏！\n");
            break;
        }

        printf("请输入w(上) s(下) a(左) d(右)\n");

        int i = 0;
        scanf("%c", &n);
        getchar();
        switch (n)
        {
        case 'w':
            up_move(buf);
            while (i < (N / 4))
            {
                get_num(buf);
                i++;
            }
            show(buf);
            break;
        case 's':
            down_move(buf);
            while (i < (N / 4))
            {
                get_num(buf);
                i++;
            }
            show(buf);
            break;
        case 'a':
            left_move(buf);
            while (i < (N / 4))
            {
                get_num(buf);
                i++;
            }
            show(buf);
            break;
        case 'd':
            right_move(buf);
            while (i < (N / 4))
            {
                get_num(buf);
                i++;
            }
            show(buf);
            break;
        default:
            break;
        }
    }

    return 0;
}
