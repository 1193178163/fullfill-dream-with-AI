/*********************************************************************
lcd图片模块：lcd_bmp.c lcd_bmp.h
	功能：
		1）指定位置显示指定大小bmp图片（lcd、mmp）
		2）图片显示特效（左滑、右滑、百叶窗、上滑，下滑，缩略图左中右显示等）
*********************************************************************/

#include "lcd_bmp.h"

/*在(s_x, s_y)位置把宽为win，高为high的bmp图片，以se的显示特效在lcd屏上显示出来*/
int Show_Bmp(int win, int high, char *picname, int s_x, int s_y, int se)
{
	//1 读取图片的数据 -->放入缓冲区
		//1) 打开图片文件
		FILE *bmp_fp = fopen(picname, "r");
		if (NULL == bmp_fp)
		{
			perror("fopen bmp failed");
			return -1;
		}
		//2) 读取图片数据 --> 前54字节不需要
		char buf[win*high*3];			//存放BMP图片数据
		fseek(bmp_fp, 54, SEEK_SET);	//偏移54字节
		fread(buf, 3, win*high, bmp_fp);	//读取图片数据
        //3) RGB --> ARGB
		int bmp_buf[win*high];		//存放转换之后的ARGB数据
        int i = 0;
		for (i = 0; i <win *high; ++i)
		{
			bmp_buf[i] = buf[3*i+2]<<16 | buf[3*i+1]<<8 | buf[3*i];
		}
        //4) 图片的像素点进行倒立
		int j;	
		for(i=0; i<high/2; i++)	//第i行数据
		{
			for(j=0; j<win; j++)	//第j列数据
			{
				//把第0 ~ 239行与 第479 ~ 240行数据进行交换
				int tmp = bmp_buf[win*i+j];
				bmp_buf[win*i+j] = bmp_buf[win*(high-1-i)+j];
				bmp_buf[win*(high-1-i)+j] = tmp;
			}
		}
	//2 写入图片数据
		//1) 打开LCD文件
		int lcd_fd = open(LCD_PATH, O_RDWR);
		if (-1 == lcd_fd)
		{
			perror("open lcd failed");	
			return -1;		
		}
		//2）写入转换之后的ARGB数据到LCD ==> 内存映射方式
		int *lcdmap = mmap(NULL, 480*800*4, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
		if (lcdmap == MAP_FAILED)
		{
			perror("mmap failed");
			return -1;
		}
    //3 显示特效
        switch(se)
        {
            case NOTHING: //无特效
                for (i = 0; i < high; i++)
				{
					for (j = 0; j < win; j++)
					{
						lcdmap[(i+s_y)*800+s_x+j] = bmp_buf[i*win+j];
					}
				}
				break;
            case LEFT_SLIP: //左滑
                for(i = win-1; i>=0; i--)   //列数
                {
                    for(j = 0; j < high; j++)    //行数
                    {
                        lcdmap[j*800+i] = bmp_buf[j*800+i];
                    }
                    usleep(100); //每显示一列像素点，延时0.01ms
                }
				break;
            case RIGHT_SLIP: //右滑
                for(i = 0; i < win; i++) //列数
                {
                    for(j = 0; j < high; j++)    //行数
                    {
                        lcdmap[j*800+i] = bmp_buf[j*800+i];
                    }
                    usleep(100); //每显示一列像素点，延时0.01ms
                }
				break;
            case WINDOW_SHADES:  //百叶窗
                for(i = 0; i < high/4; i++)    //行数
                {
                    for(j = 0; j < win; j++)   //列数
                    {
                        lcdmap[i*800+j] = bmp_buf[(i-s_y)*800+j];
                        lcdmap[(i+120)*800+j] = bmp_buf[(i+120)*800+j];
                        lcdmap[(i+240)*800+j] = bmp_buf[(i+240)*800+j];
                        lcdmap[(i+360)*800+j] = bmp_buf[(i+360)*800+j];
                    }
                    usleep(4000); //每显示一行像素点，延时4ms
                }
				break;
			case UP_SLIP:	//从下到上滑动
				for(i = high-1; i>0; i--)
				{
					for(j = 0; j<win; j++)
					{
						lcdmap[i*800+j] = bmp_buf[i*800+j];
					}
					usleep(100);	//每显示一行像素点，延时0.01ms
				}
				break;
			case DOWN_SLIP:	//从上到下滑动
				for(i = 0; i<high; i++)
				{
					for(j = 0; j<win; j++)
					{
						lcdmap[i*800+j] = bmp_buf[i*800+j];	
					}
					usleep(100);	//每显示一行像素点，延时0.01ms
				}
				break;
			case THUMBNAIL_LEFT:	//缩略图界面左边图片显示（缩小一半并只显示右3/4像素点）
				for (i = 0; i < high/2; i++)	//行数
				{
					for (j = 0; j < win/2/4*3; j++)	//列数
					{
						lcdmap[(i+s_y)*800+s_x+j] = bmp_buf[i*2*win+j*2+win/2/4];	
					}
				}
				break;
			case THUMBNAIL_MID:		//缩略图界面中间图片显示（缩小）
				for (i = 0; i < high/2; i++)	//行数
				{
					for (j = 0; j < win/2; j++)	//列数
					{
						lcdmap[(i+s_y)*800+s_x+j] = bmp_buf[i*2*win+j*2];
					}
				}
				break;
			case THUMBNAIL_RIGHT:	//缩略图界面左边图片显示（缩小一半并只显示左3/4像素点）
				for (i = 0; i < high/2; i++)	//行数
				{
					for (j = 0; j < win/2/4*3; j++)	//列数
					{
						lcdmap[(i+s_y)*800+s_x+j] = bmp_buf[i*2*win+j*2];
					}
				}
				break;
        }
	//3 关闭图片文件，关闭LCD，清除映射
	munmap(lcdmap, 800*480*4);
	fclose(bmp_fp);
	close(lcd_fd);
}
