/*********************************************************************
lcd图片模块：lcd_bmp.c lcd_bmp.h
	功能：
		1）指定位置显示指定大小bmp图片（lcd、mmp）
		2）图片显示特效（左滑、右滑、百叶窗、上滑，下滑，缩略图左中右显示等）
*********************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

//电子相册系统的图片路径
#define BUNTTON_PATH "./images/button.bmp"	//按钮图片
#define SLIDE_UNLOCK_PATH "./images/Slide_Unlock.bmp"	//滑动解锁界面
#define ICON_PATH "./images/icon.bmp"	//滑动图标
#define EXIT_LOGO_PATH "./images/exit_logo.bmp"	//退出logo
#define EXIT_PATH "./images/exit.bmp"	//退出图片
#define BACKGROUND_PATH "./images/background.bmp"	//略缩图背景
#define CONFIRM_PATH "./images/confirm.bmp"	//确认是否删除图片
#define TRUE_PATH "./images/true.bmp"	//删除成功提示
#define FALSE_PATH "./images/false.bmp"	//输入密码错误，删除失败提示
#define JOIN_PATH "./images/join.bmp"	//输入密码界面
#define BLANK_PATH "./images/blank.bmp"	//白板
#define STAR_PATH "./images/star.bmp"	//星号

#define LCD_PATH "/dev/fb0"//lcd驱动

//使用枚举存储显示特效的方式
enum SPECIAL_EFFECTS{NOTHING, LEFT_SLIP, RIGHT_SLIP, WINDOW_SHADES, UP_SLIP, DOWN_SLIP, THUMBNAIL_LEFT, THUMBNAIL_MID,THUMBNAIL_RIGHT};

/*在(s_x, s_y)位置把宽为win，高为high的bmp图片，以se的显示特效在lcd屏上显示出来*/
int Show_Bmp(int win, int high, char *picname, int s_x, int s_y, int se);
