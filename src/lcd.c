#include <stdio.h>   	//printf scanf
#include <fcntl.h>		//open write read lseek close  	 
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>

#include "lcd.h"
#include "jpeglib.h"
#include "ts.h"
#include "lcd_bmp.h"


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

static unsigned char g_color_buf[FB_SIZE]={0};

int  lcd_fd;
int *mmap_fd;



//初始化LCD
int lcd_open(void)
{
	lcd_fd = open("/dev/fb0", O_RDWR);
	
	if(lcd_fd<0)
	{
			printf("open lcd error\n");
			return -1;
	}

	mmap_fd  = (int *)mmap(	NULL, 					//映射区的开始地址，设置为NULL时表示由系统决定映射区的起始地址
									FB_SIZE, 				//映射区的长度
									PROT_READ|PROT_WRITE, 	//内容可以被读取和写入
									MAP_SHARED,				//共享内存
									lcd_fd, 				//有效的文件描述词
									0						//被映射对象内容的起点
								);
	return lcd_fd;

}

//LCD画点
void lcd_draw_point(unsigned int x,unsigned int y, unsigned int color)
{
	if (x > 799 || y > 479)
	{
		return ;
	}
	//puts("drawing");
	*(mmap_fd+y*800+x)=color;
}

int showBmp(  int x_sta , int y_sta , const char * PahtName )
{
   
    // 打开图像文件
    FILE * fp = fopen(PahtName, "r+");
    if (fp == NULL )
    {
        fprintf( stderr , "open %s error :%s \n" , PahtName , strerror(errno));
        return -1 ;
    }
    

    // 读取图像的头部信息
    struct bitmap_header bmphead ;
    if( 1 != fread( &bmphead , sizeof( bmphead ) ,  1 , fp ))
    {
        perror("read bmp head error");
        return -1 ;
    }

    // 读取图像的信息头
    struct bitmap_info bmpinfo ;
    if( 1 != fread( &bmpinfo , sizeof( bmpinfo ) ,  1 , fp ))
    {
        perror("read bmp head error");
        return -1 ;
    }


    // 为所欲为
    printf("文件大小：%d \n" , bmphead.size );
    printf("图像的数据大小：%d \n" , bmpinfo.size_img );
    printf("图像尺寸：%dx%d \n" , bmpinfo.width , bmpinfo.height  );
    printf("图像的色深：%d \n" , bmpinfo.bit_count );


    // 计算得到BMP图像为了能被4整除而补齐的字节数
    int num = 0 ;
    if (((bmpinfo.width *3) % 4) != 0 )
    {
        num = 4 - ((bmpinfo.width *3) % 4) ;
    }
    

    // 读取图像数据
    int bgr_size =  (bmpinfo.width * 3 + num ) * bmpinfo.height  ;
    char buf_bmp [ bgr_size ] ;
    if(1 != fread( buf_bmp , bgr_size, 1 ,fp ))
    {
        perror ("read file error");
        return -1 ;
    }

    // 把读取到的 BUR 数据转换成 ARGB 
    int buf_lcd [ bmpinfo.height ] [ bmpinfo.width ] ;

    for (int y = 0; y < bmpinfo.height ; y++)
    {
        for (int x = 0; x < bmpinfo.width ; x++)
        {
            buf_lcd[bmpinfo.height - y - 1] [x] =   buf_bmp [ (x+y*bmpinfo.width)*3 + 0 + y*num ] << 0 |    // 蓝色值
                                                    buf_bmp [ (x+y*bmpinfo.width)*3 + 1 + y*num ] << 8 |    // 绿色值
                                                    buf_bmp [ (x+y*bmpinfo.width)*3 + 2 + y*num ] << 16 |   // 红色值
                                                    0x00 << 24 ;            // 灰度值
        }
    }
    

    // 把转换后得到的 ARGB 显示到屏幕上
    for (int y = x_sta ; y < bmpinfo.height + y_sta  ; y++)
    {
        for (int x = x_sta  ; x < bmpinfo.width + x_sta ; x++)
        {
            if ( x < 0 ||  x > 799 || y < 0 ||  y > 479) // 边界检测 防止段错误出现
            {
                continue;
            }
            
            *(mmap_fd + x + y * 800) = buf_lcd[y-y_sta] [x-x_sta] ;
        }
        
    }

    // 关闭图像文件    
    fclose(fp);
    
    return 0;
}

//显示正常jpg图片                                           1.jpg
int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path)  
{
	/*定义解码对象，错误处理对象*/
	struct 	jpeg_decompress_struct 	cinfo;
	struct 	jpeg_error_mgr 			jerr;	
	
	unsigned char 	*pcolor_buf = g_color_buf;
	char 	*pjpg;
	
	unsigned int 	i=0;
	unsigned int	color =0;
	//unsigned int	count =0;
	
	unsigned int 	x_s = x;
	unsigned int 	x_e ;	
	unsigned int 	y_e ;
	unsigned int	y_n	= y;
	unsigned int	x_n	= x;
	
			 int	lcd_buf[480][800] = {0};
			 int	jpg_fd;
	unsigned int 	jpg_size;


	if(pjpg_path!=NULL)
	{
		/* 申请jpg资源，权限可读可写 */	
		jpg_fd=open(pjpg_path,O_RDWR);
		
		if(jpg_fd == -1)
		{
		   printf("open %s error\n",pjpg_path);
		   
		   return -1;	
		}	
		
		/* 获取jpg文件的大小 */
		jpg_size=file_size_get(pjpg_path);	
		if(jpg_size<3000)
			return -1;
		
		/* 为jpg文件申请内存空间 */	
		pjpg = malloc(jpg_size);

		/* 读取jpg文件所有内容到内存 */		
		read(jpg_fd,pjpg,jpg_size);
	}
	else
	{
		printf("jpeg path error : %s \n", pjpg_path );
		return -1;
	}

	/*注册出错处理*/
	cinfo.err = jpeg_std_error(&jerr);

	/*创建解码*/
	jpeg_create_decompress(&cinfo);

	/*直接解码内存数据*/		
	jpeg_mem_src(&cinfo,pjpg,jpg_size);
	
	/*读文件头*/
	jpeg_read_header(&cinfo, TRUE);

	/*开始解码*/
	jpeg_start_decompress(&cinfo);	
	
	
	x_e	= x_s +cinfo.output_width;
	y_e	= y  +cinfo.output_height;	

	/*读解码数据*/
	while(cinfo.output_scanline < cinfo.output_height )
	{		
		pcolor_buf = g_color_buf;
		
		/* 读取jpg一行的rgb值 */
		jpeg_read_scanlines(&cinfo,&pcolor_buf,1);
		
		for(i=0; i<cinfo.output_width; i++)
		{
			/* 不显示的部分 */
			/* if(y_n>g_jpg_in_jpg_y && y_n<g_jpg_in_jpg_y+240)
				if(x_n>g_jpg_in_jpg_x && x_n<g_jpg_in_jpg_x+320)
				{
					pcolor_buf +=3;		
					x_n++;			
					continue;
				} */
				
			/* 获取rgb值 */
			color = 		*(pcolor_buf+2);
			color = color | *(pcolor_buf+1)<<8;
			color = color | *(pcolor_buf)<<16;	
			
			/* 显示像素点 */
			lcd_draw_point(x_n,y_n,color);
			// lcd_buf[y_n][x_n] = color ;
			
			pcolor_buf +=3;
			
			x_n++;
		}
		
		/* 换行 */
		y_n++;			
		
		x_n = x_s;
		
	}		

	/*显示图像*/
	// for(y = 0 ; y < 480 ; y ++)
	// {
	// 	for(x = 0 ; x < 800 ; x ++)
	// 	{
	// 		*(mmap_fd+x+y*800) = lcd_buf[y][x];
	// 	}
	// }
	

			
	/*解码完成*/
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	if(pjpg_path!=NULL)
	{
		/* 关闭jpg文件 */
		close(jpg_fd);	
		
		/* 释放jpg文件内存空间 */
		free(pjpg);		
	}

	return 0;
}


//LCD关闭
void lcd_close(void)
{
	
	/* 取消内存映射 */
	munmap(mmap_fd, FB_SIZE);
	
	/* 关闭LCD设备 */
	close(lcd_fd);
}

//获取jpg文件的大小
unsigned long file_size_get(const char *pfile_path)
{
	unsigned long filesize = -1;	
	struct stat statbuff;
	
	if(stat(pfile_path, &statbuff) < 0)
	{
		return filesize;
	}
	else
	{
		filesize = statbuff.st_size;
	}
	
	return filesize;
}

int AutoDisplay( int fd_ts  , P_Node img_list  )
{

	// 获取指定文件fd的标签属性
	int flag = fcntl(fd_ts, F_GETFL);
		
	// 在其原有属性上，增添非阻塞属性
	flag |= O_NONBLOCK;
	fcntl(fd_ts, F_SETFL, flag);

	// 用一个临时指针 指向 播放列表
	P_Node tmp = img_list;

	while(1)
	{	
		
		puts("start auto play");
		// 判断 他的类型 bmp   jpg  根据类型显示图像文件
		if ( tmp->data.Type ==  'j' )
		{
			lcd_draw_jpg(0,0, tmp->data.PathName ) ;
		}
		else if ( tmp->data.Type ==  'b' )
		{
			showBmp(  0 , 0 , tmp->data.PathName ) ;
		}
		
		tmp = tmp->Next ;
		
		// 检查出触摸屏是否被操作
		if(touch_test( fd_ts ))
		{
			// 获取指定文件fd的标签属性
			int flag = fcntl(fd_ts, F_GETFL);
				
			// 在其原有属性上，增添非阻塞属性
			flag &= !O_NONBLOCK;
			fcntl(fd_ts, F_SETFL, flag);
			puts("outs!");
			return 0;
		}
		

		// 获取列表中的下一个资源 
		if (tmp == img_list )  // 跳过头节点
		{
			tmp = tmp->Next ;
		}
	
	}


	return 0 ;
}

int gamer(void)
{
	int fp_ts=lcd_open();
    int fd_ts =Init_Touch();
	showBmp(0,0,BLANK_PATH);
    showBmp(0,0,STAR_PATH);
    // 打开图像文件
    FILE * fp = fopen("./images/bigking.bmp", "r+");
    if (fp == NULL )
    {
        fprintf( stderr , "open %s error :%s \n" , "./images/bigking.bmp" , strerror(errno));
        return -1 ;
    }
    

    // 读取图像的头部信息
    struct bitmap_header bmphead ;
    if( 1 != fread( &bmphead , sizeof( bmphead ) ,  1 , fp ))
    {
        perror("read bmp head error");
        return -1 ;
    }

    // 读取图像的信息头
    struct bitmap_info bmpinfo ;
    if( 1 != fread( &bmpinfo , sizeof( bmpinfo ) ,  1 , fp ))
    {
        perror("read bmp head error");
        return -1 ;
    }


    // 为所欲为
    printf("文件大小：%d \n" , bmphead.size );
    printf("图像的数据大小：%d \n" , bmpinfo.size_img );
    printf("图像尺寸：%dx%d \n" , bmpinfo.width , bmpinfo.height  );
    printf("图像的色深：%d \n" , bmpinfo.bit_count );


    // 计算得到BMP图像为了能被4整除而补齐的字节数
    int num = 0 ;
    if (((bmpinfo.width *3) % 4) != 0 )
    {
        num = 4 - ((bmpinfo.width *3) % 4) ;
    }
    

    // 读取图像数据
    int bgr_size =  (bmpinfo.width * 3 + num ) * bmpinfo.height  ;
    char buf_bmp [ bgr_size ] ;
    if(1 != fread( buf_bmp , bgr_size, 1 ,fp ))
    {
        perror ("read file error");
        return -1 ;
    }

    // 把读取到的 BUR 数据转换成 ARGB 
    int buf_lcd [ bmpinfo.height ] [ bmpinfo.width ] ;

    for (int y = 0; y < bmpinfo.height ; y++)
    {
        for (int x = 0; x < bmpinfo.width ; x++)
        {
            buf_lcd[bmpinfo.height - y - 1] [x] =   buf_bmp [ (x+y*bmpinfo.width)*3 + 0 + y*num ] << 0 |    // 蓝色值
                                                    buf_bmp [ (x+y*bmpinfo.width)*3 + 1 + y*num ] << 8 |    // 绿色值
                                                    buf_bmp [ (x+y*bmpinfo.width)*3 + 2 + y*num ] << 16 |   // 红色值
                                                    0x00 << 24 ;            // 灰度值
        }
    }
    
	int x_sta=100,y_sta=100; 
	fd_ts=Init_Touch();
	while(1)
    {
        
		// 读取触摸屏文件的数据
        struct input_event event ; // 定义一个结构体来存储读取到的数据
        ssize_t ret_val = read(fd_ts , &event , sizeof(event));
        printf("成功读取%d 字节.\n" , ret_val );


        // 分析数据得到坐标值 
        printf("Type:%d \t Code:%d \t  Value:%d\n" ,event.type , event.code ,event.value );
        

        if (event.type == EV_ABS ) // 判断当前读取的数据中类型为触摸屏类型则进一步判断
        {
            if (event.code == ABS_X )
            {
                x_sta = event.value*800/1024 ;
            }
            else if (event.code == ABS_Y )
            {
                y_sta = event.value*480/600 ;
            }
            
        }
        
		// 把转换后得到的 ARGB 显示到屏幕上
		for (int y = y_sta ; y <  y_sta+20  ; y++)
		{
			for (int x = x_sta  ; x < 20+ x_sta ; x++)
			{
				if ( x < 0 ||  x > 799 || y < 0 ||  y > 479) // 边界检测 防止段错误出现
				{
					continue;
				}
				
				*(mmap_fd + x + y * 800) = buf_lcd[y] [x] ;
			}
			
		}
        //gamer(x,y,"./bmps/23.bmp");
        //Show_Bmp(5,5,"./images/point01.bmp",x,y,NOTHING);
        // if(x_sta>700&&y_sta>360)
        //     return 0;
        if(x_sta<40&&x_sta>10&&y_sta>10&&y_sta<40)
        {
            if (Administrator_Interface())
            {
                /* code */
                //gamer("./images/bigking.bmp",fd_ts);  //刮刮乐
                game_meun();
            }
            
            return 0;
        }
            
    }


    

    // 关闭图像文件    
    fclose(fp);
    
    return 0;
}