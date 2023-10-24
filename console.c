#include <stdio.h>
#include <stdlib.h>
#include "console.h"

void cusor_moveto(int x, int y)
{// ESC[y;xH
    printf("\033[%d;%dH",y,x);
    fflush(stdout);
} 

//保存光标位置
void cusor_get_pos(void)
{// ESC[s
    printf("\033[s");
    fflush(stdout);
} 

//恢复光标位置
void cusor_set_pos(void)
{// ESC[u
    printf("\033[u");
    fflush(stdout);
} 
void cusor_hide(void)
{
	printf("\033[?25l");
}
//清屏
void clear_screen(void)
{// ESC[2J
    printf("\033[2J");
    fflush(stdout);
}

/*
COLOR_RED              红
COLOR_BLACK            黑
COLOR_GREEN            绿
COLOR_BLUE             蓝
COLOR_YELLOW           黄
COLOR_WHITE            白
COLOR_CYAN             青
COLOR_MAGENTA          洋红
*/
//设置前景颜色
void set_fg_color(int color)
{// ESC[#m
    printf("\033[%dm",color);
    fflush(stdout);
}

//设置背景颜色
void set_bg_color(int color)
{// ESC[#m
    printf("\033[%dm",(color+10));
    fflush(stdout);
}