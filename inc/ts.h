#ifndef     TS_H
#define     TS_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <linux/input.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "MyTypes.h"
#include "list.h"

#define TS_EVENT "/dev/input/event0"	//触摸屏驱动

/*初始化触摸屏*/
int Init_Touch( void );

/*获取触摸屏坐标信息*/
int getxy(int * x , int * y , int fd_ts );

/*触摸屏检测*/
int touch_test(  int fd_ts );

/*滑动解锁界面*/
void Slide_Unlock();

/*电子相册主界面*/
void Album();

/*缩略图界面*/
void Thumbnail_Interface();

/*画板*/
void Slid(int fd_ts);

/*显示管理员密码输入界面，并删除图片在链表中的路径*/
bool Administrator_Interface();

#endif