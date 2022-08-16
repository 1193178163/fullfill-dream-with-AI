#ifndef     BALL_H
#define     BALL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <linux/input.h>
#include <pthread.h>
#include <errno.h>

#define TS_EVENT "/dev/input/event0"	//触摸屏驱动
#define LCD_PATH "/dev/fb0"//lcd驱动

int game_meun(void);


#endif