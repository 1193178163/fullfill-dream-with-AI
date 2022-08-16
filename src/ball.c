#include "ball.h"

int lcd_fd;
int *mmap_fd;
unsigned int *FB;
int ts_fd;
int g_x;
int g_y;
int total;

// BMP格式头规范
struct bitmap_header
{
	int16_t type;
	int32_t size; // 图像文件大小
	int16_t reserved1;
	int16_t reserved2;
	int32_t offbits; // bmp图像数据偏移量
}__attribute__((packed));

struct bitmap_info
{
	int32_t size;   // 本结构大小	
	int32_t width;  // 图像宽
	int32_t height; // 图像高
	int16_t planes;

	int16_t bit_count; // 色深
	int32_t compression;
	int32_t size_img; // bmp数据大小，必须是4的整数倍
	int32_t X_pel;
	int32_t Y_pel;
	int32_t clrused;
	int32_t clrImportant;
}__attribute__((packed));

//显示色块
void show_color(int x_start,int y_start,int w,int h, int color)
{
    int x,y;
    for(y=y_start;y<y_start+h;y++)
    {
        for(x=x_start;x<x_start+w;x++)
        {
            *(FB+x+800*y)= color;           //映射色块
        }
    }
}

// int showBmp(  int x_sta , int y_sta , const char * PahtName )
// {
   
//     // 打开图像文件
//     FILE * fp = fopen(PahtName, "r+");
//     if (fp == NULL )
//     {
//         fprintf( stderr , "open %s error :%s \n" , PahtName , strerror(errno));
//         return -1 ;
//     }
    

//     // 读取图像的头部信息
//     struct bitmap_header bmphead ;
//     if( 1 != fread( &bmphead , sizeof( bmphead ) ,  1 , fp ))
//     {
//         perror("read bmp head error");
//         return -1 ;
//     }

//     // 读取图像的信息头
//     struct bitmap_info bmpinfo ;
//     if( 1 != fread( &bmpinfo , sizeof( bmpinfo ) ,  1 , fp ))
//     {
//         perror("read bmp head error");
//         return -1 ;
//     }


//     // 为所欲为
//     printf("文件大小：%d \n" , bmphead.size );
//     printf("图像的数据大小：%d \n" , bmpinfo.size_img );
//     printf("图像尺寸：%dx%d \n" , bmpinfo.width , bmpinfo.height  );
//     printf("图像的色深：%d \n" , bmpinfo.bit_count );


//     // 计算得到BMP图像为了能被4整除而补齐的字节数
//     int num = 0 ;
//     if (((bmpinfo.width *3) % 4) != 0 )
//     {
//         num = 4 - ((bmpinfo.width *3) % 4) ;
//     }
    

//     // 读取图像数据
//     int bgr_size =  (bmpinfo.width * 3 + num ) * bmpinfo.height  ;
//     char buf_bmp [ bgr_size ] ;
//     if(1 != fread( buf_bmp , bgr_size, 1 ,fp ))
//     {
//         perror ("read file error");
//         return -1 ;
//     }

//     // 把读取到的 BUR 数据转换成 ARGB 
//     int buf_lcd [ bmpinfo.height ] [ bmpinfo.width ] ;

//     for (int y = 0; y < bmpinfo.height ; y++)
//     {
//         for (int x = 0; x < bmpinfo.width ; x++)
//         {
//             buf_lcd[bmpinfo.height - y - 1] [x] =   buf_bmp [ (x+y*bmpinfo.width)*3 + 0 + y*num ] << 0 |    // 蓝色值
//                                                     buf_bmp [ (x+y*bmpinfo.width)*3 + 1 + y*num ] << 8 |    // 绿色值
//                                                     buf_bmp [ (x+y*bmpinfo.width)*3 + 2 + y*num ] << 16 |   // 红色值
//                                                     0x00 << 24 ;            // 灰度值
//         }
//     }
    

//     // 把转换后得到的 ARGB 显示到屏幕上
//     for (int y = x_sta ; y < bmpinfo.height + y_sta  ; y++)
//     {
//         for (int x = x_sta  ; x < bmpinfo.width + x_sta ; x++)
//         {
//             if ( x < 0 ||  x > 799 || y < 0 ||  y > 479) // 边界检测 防止段错误出现
//             {
//                 continue;
//             }
            
//             *(mmap_fd + x + y * 800) = buf_lcd[y-y_sta] [x-x_sta] ;
//         }
        
//     }

//     // 关闭图像文件    
//     fclose(fp);
    
//     return 0;
// }

//显示ebm格式的图片
void show_ebm(const char *pic)
{
    int pic_fd = open(pic,O_RDONLY);
    if(pic_fd == -1)
    {
        perror("open pic failed!");
        return;
    }

    unsigned int pic_buf[800*480]={0};
    read(pic_fd,pic_buf,sizeof(pic_buf));

    int x,y;
    for(y=0;y<480;y++)
    {
        for(x=0;x<800;x++)
        {
            *(FB+x+800*y)=pic_buf[x+800*y];
        }
    }

}

//打开lcd
void lcd_init(void)
{
    lcd_fd = open("/dev/fb0",O_RDWR);
    if(lcd_fd == -1)
    {
        perror("open failed!");
        return;
    }

    ts_fd = open("/dev/input/event0",O_RDONLY);
    if(ts_fd == -1)
    {
        perror("open ts failed!");
        return;
    }

    FB = mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,lcd_fd,0);
    if(mmap == MAP_FAILED)
    {
        perror("mmap open failed!");
        return; 
    }

}

void lcd_colse(void)
{ 
    close(lcd_fd);
    munmap(FB,800*480*4);
    close(ts_fd);
}

//获得触摸屏XY的值
void get_xy(void)
{
    //1, 打开触摸屏文件
	int ts_fd = open(TS_EVENT, O_RDWR);
	if (-1 == ts_fd)
	{
		perror("open ts failed");
		return;
	}
    
    struct input_event ts_buf;
    while (1)
    {
        //读取
        read(ts_fd, &ts_buf, sizeof(ts_buf));
        if(ts_buf.type == EV_ABS && ts_buf.code == ABS_X)   //获取x的值
        {
            g_x = ts_buf.value*800/1024;
        }

        if(ts_buf.type == EV_ABS && ts_buf.code == ABS_Y)   //获取Y的值
        {
            g_y = ts_buf.value*480/600;
        }

        if(ts_buf.type == EV_KEY && ts_buf.code == BTN_TOUCH && ts_buf.value == 0)  //松手结束
        {
            break;
        }
    }
    
}

//获得木板函数（调用到触摸屏函数）
void plank(int touch_x)
{
    int x,y;
    for(y=400;y<450;y++)
    {
        for(x=0;x<800;x++)
        {
            //点击的区域是木板的中点
            if(x>touch_x-80 && x<touch_x+80)
            {
                *(FB+x+800*y) = 0xFF0000;//红色的木板
            }

            else 
            {
               *(FB+x+800*y) = 0xFFFFFF;//其他区域是白色
            }
            
        }
    }
}

//画球
void *mv_ball(void *rgb)
{
    //找到圆心
    int x0 = 400;
    int y0 = 240;

    //球的半径
    int r = 60;

    int x,y;
    int flge_x=0;
    int flge_y=0; //0就--；1就++
    int cnt = 0;
    int time = 2000;
    while (1)
    {
        //内切圆获取圆的圆心
        for(y=y0-r;y<=y0+r;y++)
        {
            for(x=x0-r;x<=x0+r;x++)
            {
                if((x-x0)*(x-x0) + (y-y0)*(y-y0) <= r*r)
                {
                    *(FB+x+800*y) = 0xFFFF00;//画球
                }

                else
                {
                    *(FB+x+800*y) = 0xFFFFFF;
                }
            }
        }

        usleep(time);

        //如果球碰到479游戏结束
        if(y0 + r ==479)
        {
            //sleep(1);
            //show_pic("gov.ebm");
            pthread_exit(NULL);

        }

        //判断球是否碰到木板
        if(x>g_x-100 && x<g_x+100 && y0 + r ==400)
        {
            plank(g_x);
            flge_y = 0;
            cnt = 1;
            cnt ++;
            if(cnt ++)
            {
                time -= 100;
            }
        }

        if(x0 + r ==799)
        {
            flge_x = 0;
        }

        if(y0 - r == 0)
        {
            flge_y = 1;
        }

        if(x0 - r == 0)
        {
            flge_x = 1;
        }


        if(flge_x == 0)
            x0--;
        if(flge_x == 1)
            x0++;
        if(flge_y == 0)
            y0--;
        if(flge_y == 1)
            y0++;            

    

    }
    
    return 0;
}

//触摸屏获取木板
void *get_plank(void *rgb)
{
    struct input_event ts_buf;

    while (1)
    {
        read(ts_fd,&ts_buf,sizeof(ts_buf));
        if(ts_buf.type==EV_ABS && ts_buf.code==ABS_X)
        {
            g_x=ts_buf.value*800/1024;

        }
        if(ts_buf.type==EV_ABS && ts_buf.code==ABS_Y)
        {
            g_y=ts_buf.value*480/600;

        }

        if(g_y>400 && g_y<450 && g_x>0 && g_x<800)
        {
            plank(g_x);
        }

    
    }

    return 0;
    
}

//游戏运行=
int game_run(void)
{
    //定义两个线程的id
    pthread_t ball_id,get_id;
    int rte1=  pthread_create(&ball_id,NULL,mv_ball,NULL);//球
    int rte2= pthread_create(&get_id,NULL,get_plank,NULL);//木板

    if(rte1 !=0 || rte2 !=0)
    {
        perror("pthread failed");
        return 1;
    }

    pthread_join(ball_id,NULL);//回收球的线程
    pthread_cancel(get_id);//销毁木板的线程
    pthread_join(get_id,NULL);//回收木板的线程
    
    

    return 0;
}


//显示游戏的菜单
int game_meun(void)
{
    lcd_init();
    show_ebm("yx.ebm");
    while (1)
    {
        get_xy();

        if(g_x>380 && g_x<580 && g_y>160 &&g_y<240)             //点击此区域，进入游戏的进度条，再全屏刷白，再进入游戏的运行程序
        {
            //游戏进度条
            //game_laoding();

            //全屏刷白
            show_color(0,0,800,480,0xFFFFFF);

            //游戏运行
            game_run();

            //显示游戏界面
            show_ebm("yx.ebm");

            //游戏主菜单
            game_meun();
        }

        if(g_x>320 && g_x<650 && g_y>260 &&g_y<350)             //点击此区域，显示主界面图片，进入主菜单
        {
            // //显示程序的主菜单界面
            // showBmp(0,0,"yx.bmp");

            // //显示程序的主菜单
            // show_maue(head);

            break;
            return 0;
        }


      
    }


    return 0;
    
}

// int main(int argc, char const *argv[])
// {
//     lcd_init();
//     show_ebm("yx.ebm");
//     game_meun();
//     return 0;
// }


//加载运行的进度条
// int game_laoding(void)
// {
//     int i;

//     show_color(0,0,800,480,0xFFFFFF);                               //全屏刷白
//     show_color(230,234,308,5,0xFF0000);                               //用红色色块显示进度条的框
//     show_color(230,238,5,39,0xFF0000);
//     show_color(230,273,308,5,0xFF0000);
//     show_color(533,238,5,39,0xFF0000);
//     Display_characterX(270,184,"Game Loading...",0xFF0000,2);           //字体显示

//     for(i=1;i<300;i++)
//     {
//         show_color(235,238,i,35,0x00FF00);                          //绿色的进度条
//         usleep(10000);
               
//     } 
//     show_color(0,0,800,480,0xFFFFFF);                               //全刷白

//     return 0;    

// }



