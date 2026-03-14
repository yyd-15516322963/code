#include <ncurses.h>

/*
	ncurses库:开源库,用来显示字符终端,读取键盘输入,显示
	   1.我不敲回车,也能读取字符
	     正常情况下scanf("%c",&n); //敲回车才结束读取
		 正常情况下
	   2.ncurses库提供一些接口函数给程序调用,学习的时候掌握接口函数的作用即可
	   3.整理步骤
	      (1)第一步:在ubuntu安装ncurses库
		        sudo apt-get update   //更新下载源
				sudo apt-get install -y libncurses5-dev libncursesw5-dev //下载ncurses库
		  (2)写代码使用ncurses库提供的接口函数
		     彭老师抛砖引玉介绍了其中一部分接口函数,其余的你自己拓展(感兴趣的话)
			 编译程序: gcc ncurses库.c   -lncurses
*/
int main() 
{
	//C语言标准库没有这些函数,这些函数全部都是ncurses库提供的
    initscr(); // 初始化ncurses模式
    noecho();  // 不显示输入的字符
    cbreak();  // 禁用行缓冲，输入的字符立即处理
    printw("pls input char: ");
	while(1)
	{
		refresh(); // 刷新屏幕以显示消息
		int ch = getch(); // 读取一个字符
		printw("you input is: %c\n", ch);
	}
    endwin();   // 结束ncurses模式
    return 0;
}