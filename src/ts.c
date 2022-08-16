#include "ts.h"
#include "lcd_bmp.h"
#include "Mytypes.h"
#include "lcd.h"
#include "dir.h"
#include "ball.h"

#define TOUCH_PAHT "/dev/input/event0"

extern P_Node BMP;

int Init_Touch( void )
{

    // 打开触摸屏所对应的设备文件
    int fd = open(TOUCH_PAHT , O_RDWR );
    if (fd == -1 )
    {
        perror("open touch error");
        return -1 ;
    }

    return fd ;
}

int getxy(int * x , int * y , int fd_ts )
{


    while(1)
    {
        // 读取触摸屏文件的数据
        struct input_event event ; // 定义一个结构体来存储读取到的数据
        ssize_t ret_val = read(fd_ts , &event , sizeof(event));
        printf("成功读取%d 字节.\n" , ret_val );


        // 分析数据得到坐标值 
        // printf("Type:%d \t Code:%d \t  Value:%d\n" ,event.type , event.code ,event.value );
        

        if (event.type == EV_ABS ) // 判断当前读取的数据中类型为触摸屏类型则进一步判断
        {
            if (event.code == ABS_X )
            {
                *x = event.value*800 / 1024 ;
                printf("当前X轴坐标为：%d\n" , event.value);
            }
            else if (event.code == ABS_Y )
            {
                *y = event.value*480 / 600 ;
                printf("当前Y轴坐标为：%d\n" , event.value);
            }
        }
        
        // 判断是否松手
        if (event.type == EV_KEY && event.code == BTN_TOUCH && event.value == 0 )
        {
            printf("手指离开触摸屏.....\n");
            break ;
        }
        
    }

    return 0;
}

/*获取点击坐标，x1和y1分别是触摸屏当前的x，y坐标， x2和y2分别是松手后x，y坐标*/
void Get_XY(int *x1, int *y1, int *x2, int *y2)
{
	//1, 打开触摸屏文件
	int ts_fd = open(TS_EVENT, O_RDWR);
	if (-1 == ts_fd)
	{
		perror("open ts failed");
		return;
	}
	//2, 循环读取触摸屏文件的数据 ==>读取数据按照什么格式？ ==> 输入子系统
	struct input_event tsevent;  //输入事件结构体
	int flag1=1, flag2 = 1;
	int x, y;
	while(1)
	{
		//读取你点击触摸屏的坐标
		//触摸屏如果你没有点击，read无法读取到坐标值，会一直阻塞
		read(ts_fd, &tsevent, sizeof(tsevent));
		//判断事件类型
		if(tsevent.type==EV_ABS)  //说明你触发的是触摸屏事件
		{
			if(tsevent.code==ABS_X)  //x坐标
			{
				x = tsevent.value*800/1024; //不断记录目前新的坐标
				if(flag1) //保证起始位置坐标只读一次
				{
					flag1 = 0;
					*x1=x;
				} 
			}
				
			if(tsevent.code==ABS_Y)  //y坐标
			{
				y = tsevent.value*480/600;
				if(flag2)
				{
					flag2 = 0;
					*y1=y;
				} 
			}
		}
		//手松开
		if(tsevent.type==EV_KEY&&tsevent.code==BTN_TOUCH&&tsevent.value==0)
		{
			*x2=x;
			*y2=y;
			break;
		}
	}
	close(ts_fd);
}

int touch_test(  int fd_ts )
{

    // 设置触摸屏为非阻塞属性
    // 获取指定文件fd的标签属性
    int flag = fcntl(fd_ts, F_GETFL);
		
    // 在其原有属性上，增添非阻塞属性
    flag |= O_NONBLOCK;
    fcntl(fd_ts, F_SETFL, flag);

    // 读取触摸屏文件的数据
    struct input_event event ; // 定义一个结构体来存储读取到的数据
    ssize_t ret_val = read(fd_ts , &event , sizeof(event));

    struct timeval tv;
    struct timezone tz;
    gettimeofday (&tv, &tz);
    long sec_tmp = tv.tv_sec ;
    printf("当前时间：%x\n" , sec_tmp) ;
    // sleep(2);
		
    while(1)
    {
        ssize_t ret_val = read(fd_ts , &event , sizeof(event));
		
        if (ret_val <= 0  )
        {
           
            gettimeofday (&tv, &tz);
            if (tv.tv_sec == sec_tmp + 2)
            {
                return 0 ;
            }
        }
        else if ( ret_val > 0 )
        {
            return 1 ;
        }
        
		    

    }



}

void Slide_Unlock()
{
    //显示解锁界面
    //showBmp(0,0,SLIDE_UNLOCK_PATH);
    Show_Bmp(800,480,SLIDE_UNLOCK_PATH,0,0,DOWN_SLIP);
    Show_Bmp(100,50,ICON_PATH,100,350,NOTHING);

    //打开触摸屏文件
    int fd = Init_Touch();

    //读取触摸屏数据
    struct input_event xy;  //输入事件结构体
    int x, y;
    int icon_flag = 0;  //滑动图片标志位，0为没点击滑动图标，1为已点击滑动图标
    int unlock_flag = 0;    //是否解锁成功标志位，0为为解锁， 1为解锁成功
	while(1)
	{
		read(fd, &xy, sizeof(xy));
        if(xy.type==EV_ABS)  //说明你触发的是触摸屏事件
        {
            if(xy.code == ABS_X)
            {
                x = xy.value*800/1024;	//触摸屏坐标修正
            }
            if(xy.code == ABS_Y)
            {
                y = xy.value*480/600;	//触摸屏坐标修正
            }
            if(x>=100 && x<200 && y>=350 && y<400)
            {
                icon_flag = 1;
            }
            if(icon_flag == 1 && x>=150 && x<650)
            {
                //Show_Bmp(800, 480, SLIDE_UNLOCK_PATH, 0, 0, NOTHING);
                Show_Bmp(100, 50, ICON_PATH, x-0.5, 350, NOTHING);
                if(x>=600)
                {
                    //Show_Bmp(800, 480, SLIDE_UNLOCK_PATH, 0, 0, NOTHING);
                    Show_Bmp(100, 50, ICON_PATH, 600, 350, NOTHING);
                    icon_flag = 0;
                    unlock_flag = 1;
                }
            }
        }
        //松手未解锁成功
        if(xy.type == EV_KEY && xy.code == BTN_TOUCH && xy.value == 0 && unlock_flag == 0)
        {
            Show_Bmp(800, 480, SLIDE_UNLOCK_PATH, 0, 0, NOTHING);
            Show_Bmp(100, 50, ICON_PATH, 100, 350, NOTHING);
            icon_flag = 0;
        }
        //松手并成功解锁。
        if(xy.type == EV_KEY && xy.code == BTN_TOUCH && xy.value == 0 && unlock_flag == 1)
        {
            printf("滑动解锁成功！\n");
            close(fd);	
            return ;
            break;
        }
	}	

}

/*电子相册主界面*/
void Album()
{
    P_Node tmp=BMP;
    while (1)
    {
        /* code */
        sleep(1);
        Show_Bmp(800, 480, tmp->data.PathName, 0, 0, RIGHT_SLIP);
        Show_Bmp(800, 50, BUNTTON_PATH, 0, 430, NOTHING);
        tmp=tmp->Next;
    }
}


void Slid( int fd_ts )
{
    showBmp(0,0,BLANK_PATH);
    showBmp(0,0,STAR_PATH);
    int in_x , in_y; // 按下的坐标值
    int out_x ,out_y ; // 松手坐标值
	int x=100,y=100; 

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
                x = event.value*800/1024 ;
            }
            else if (event.code == ABS_Y )
            {
                y = event.value*480/600 ;
            }
            
        }
        
        // // 判断是否松手
        // if (event.type == EV_KEY && event.code == BTN_TOUCH && event.value == 0 )
        // {
        //     printf("手指离开触摸屏.....\n");
        //     out_x = x ;
        //     out_y = y ;
        //     continue ;
        // }
        // else if (event.type == EV_KEY && event.code == BTN_TOUCH && event.value > 0 )
        // {
        //     printf("手指按下触摸屏.....\n");
        //     in_x = x ;
        //     in_y = y ;
        // }
        //lcd_draw_jpg(x,y,"./images/point01.jpg");
        //showBmp(x,y,"./images/point01.bmp");
        Show_Bmp(5,5,"./images/point01.bmp",x,y,NOTHING);
        if(x<40&&x>10&&y>10&&y<40)
        {
            if (Administrator_Interface())
            {
                /* code */
                //gamer("./images/bigking.bmp",fd_ts);  //刮刮乐
                game_meun();
            }
            
            return ;
        }
            

    }

    printf("按下时坐标值：(%d,%d) 松手时坐标值：(%d,%d)\n" , in_x ,in_y , out_x , out_y);
	
	return ;
}

void draw()
{
    showBmp(0,0,BLANK_PATH);
    showBmp(0,0,STAR_PATH);
    int fd=lcd_open();
    int fp =Init_Touch();

    Slid(fp);
    //gamer("./images/bigking.bmp",fp);

    //showBmp(x,y,"./images/point01.bmp");

}

/*缩略图界面*/
void Thumbnail_Interface()
{
    //0. 显示略缩图界面的背景
    Show_Bmp(800, 480, BACKGROUND_PATH, 0, 0, NOTHING);
    P_Node BMPhead=BMP; 

    if(Bmp_Display(BMP) == 1)    //只有一张图片的情况
    {
        Show_Bmp(800, 480, BMP->data.PathName, 0, 100, THUMBNAIL_LEFT);
        Show_Bmp(800, 480, BMP->data.PathName, 500, 100, THUMBNAIL_RIGHT);
        Show_Bmp(800, 480, BMP->data.PathName, 200, 130, THUMBNAIL_MID);
    }
    else if(Bmp_Display(BMPhead) == 2)    //只有2张图片的情况
    {
        if(BMP->Next == BMPhead)
        {
            Show_Bmp(800, 480, BMP->Next->Next->data.PathName, 0, 100, THUMBNAIL_LEFT);
            Show_Bmp(800, 480, BMP->Next->Next->data.PathName, 500, 100, THUMBNAIL_RIGHT);
        }
        else
        {
            Show_Bmp(800, 480, BMP->Next->data.PathName, 0, 100, THUMBNAIL_LEFT);
            Show_Bmp(800, 480, BMP->Next->data.PathName, 500, 100, THUMBNAIL_RIGHT);
        }
        Show_Bmp(800, 480, BMP->data.PathName, 200, 130, THUMBNAIL_MID);
    }
    else    //三张以上的情况
    {
        if (BMP->Prev==BMPhead) //修正：跳过链表的空表头,显示缩略图左边图片
            {
                Show_Bmp(800, 480, BMP->Prev->Prev->data.PathName, 0, 100, THUMBNAIL_LEFT);
            }
            else
            {
                Show_Bmp(800, 480, BMP->Prev->data.PathName, 0, 100, THUMBNAIL_LEFT);
            }
            if (BMP->Next==BMPhead) //修正：跳过链表的空表头,显示缩略图右边图片
            {
                Show_Bmp(800, 480, BMP->Next->Next->data.PathName, 500, 100, THUMBNAIL_RIGHT);
            }
            else
            {
                Show_Bmp(800, 480, BMP->Next->data.PathName, 500, 100, THUMBNAIL_RIGHT);
            }
            Show_Bmp(800, 480, BMP->data.PathName, 200, 130, THUMBNAIL_MID); //显示缩略图中间图片
    }
    //2. 根据不同的操作更新界面
    int x1, y1, x2, y2;
    while(1)
    {
        Get_XY(&x1, &y1, &x2, &y2);
        if(((x2-x1)>0 && abs(x2-x1)>abs(y2-y1)) || (x1<200 && y1<340 && y1>=100))  //右滑或点击左边显示上一张略缩图片
        {
            if(BMP->Prev == BMPhead)   //修正：跳过链表的空表头
            {
                BMP = BMP->Prev;
            }
            BMP = BMP->Prev;
            
            printf("上一张略缩图片:%s\n", BMP->data.PathName);
            if (BMP->Prev == BMPhead) //修正：跳过链表的空表头,显示缩略图左边图片
            {
                Show_Bmp(800, 480, BMP->Prev->Prev->data.PathName, 0, 100, THUMBNAIL_LEFT);
            }
            else
            {
                Show_Bmp(800, 480, BMP->Prev->data.PathName, 0, 100, THUMBNAIL_LEFT);
            }
            if (BMP->Next == BMPhead) //修正：跳过链表的空表头,显示缩略图右边图片
            {
                Show_Bmp(800, 480, BMP->Next->Next->data.PathName, 500, 100, THUMBNAIL_RIGHT);
            }
            else
            {
                Show_Bmp(800, 480, BMP->Next->data.PathName, 500, 100, THUMBNAIL_RIGHT);
            }
            Show_Bmp(800, 480, BMP->data.PathName, 200, 130, THUMBNAIL_MID); //显示缩略图中间图片
        }
        else if (((x2-x1)<0 && abs(x2-x1)>abs(y2-y1)) || (x1>600 && y1<340 && y1>=100))  //左滑或点击右边显示下一张略缩图片
        {
            if (BMP->Next == BMPhead)   //修正：跳过链表的空表头
            {
                BMP = BMP->Next;
            }
            BMP = BMP->Next;
            
            printf("下一张略缩图片:%s\n", BMP->data.PathName);
            if (BMP->Prev == BMPhead) //修正：跳过链表的空表头，显示左边略缩图片
            {
                Show_Bmp(800, 480, BMP->Prev->Prev->data.PathName, 0, 100, THUMBNAIL_LEFT);
            }
            else
            {
                Show_Bmp(800, 480, BMP->Prev->data.PathName, 0, 100, THUMBNAIL_LEFT);
            }
            if (BMP->Next == BMPhead) //修正：跳过链表的空表头
            {
                Show_Bmp(800, 480, BMP->Next->Next->data.PathName, 500, 100, THUMBNAIL_RIGHT);
            }
            else
            {
                Show_Bmp(800, 480, BMP->Next->data.PathName, 500, 100, THUMBNAIL_RIGHT);
            }
            Show_Bmp(800, 480, BMP->data.PathName, 200, 130, THUMBNAIL_MID);
        }
        else if (x1>=200 && x1<600 && y1>=130 && y1<370) //点击中间图片，放大成普通图片
        {
            printf("放大图片\n");
            Show_Bmp(800,480, BMP->data.PathName, 0, 0, NOTHING);
            while(1)
            {
                int fd_ts=Init_Touch();
                AutoDisplay( fd_ts  , BMP  );
                Get_XY(&x1, &y1, &x2, &y2);
                if((x2-x1)>0 && abs(x2-x1)>abs(y2-y1))  //右滑显示上一张普通图片
                {
                    printf("上一张\n");
                    if(BMP->Prev == BMPhead)   //修正：跳过链表的空表头
                    {
                        BMP = BMP->Prev;
                    }
                    BMP = BMP->Prev;
                    Show_Bmp(800, 480, BMP->data.PathName, 0, 0, RIGHT_SLIP);
                }
                else if ((x2-x1)<0 && abs(x2-x1)>abs(y2-y1))    //左滑显示下一张普通图片
                {
                    printf("下一张\n");
                    if (BMP->Next == BMPhead)   //修正：跳过链表的空表头
                    {
                        BMP = BMP->Next;
                    }
                    BMP = BMP->Next;
                    Show_Bmp(800, 480, BMP->data.PathName, 0, 0, LEFT_SLIP);
                }
                else    //点击任意位置返回
                {
                    printf("返回缩略图界面\n");
                    Show_Bmp(800, 480, BACKGROUND_PATH, 0, 0, NOTHING);
                    if (BMP->Prev == BMPhead) //修正：跳过链表的空表头
                    {
                        Show_Bmp(800, 480, BMP->Prev->Prev->data.PathName, 0, 100, THUMBNAIL_LEFT);
                    }
                    else
                    {
                        Show_Bmp(800, 480, BMP->Prev->data.PathName, 0, 100, THUMBNAIL_LEFT);
                    }
                    if (BMP->Next == BMPhead) //修正：跳过链表的空表头
                    {
                        Show_Bmp(800, 480, BMP->Next->Next->data.PathName, 500, 100, THUMBNAIL_RIGHT);
                    }
                    else
                    {
                        Show_Bmp(800, 480, BMP->Next->data.PathName, 500, 100, THUMBNAIL_RIGHT);
                    }
                    Show_Bmp(800, 480, BMP->data.PathName, 200, 130, THUMBNAIL_MID);
                    break; //点击任意位置返回缩略图界面
                }
                
            }
        }
        else if((y1-y2)>0 && abs(x2-x1)<abs(y1-y2)) //上划打开画板
        {
            //draw();
            gamer();
            Thumbnail_Interface();

        }
        else if((y2-y1)>0 && abs(x2-x1)<abs(y2-y1)) //下滑返回主界面
        {
            printf("返回电子相册主界面\n");
            // Show_Bmp(800, 480, BMP->data.PathName, 0, 0, DOWN_SLIP);
            // Show_Bmp(800, 50, BUNTTON_PATH, 0, 430, NOTHING);
            Slide_Unlock();

            Thumbnail_Interface();
        }
    }
}

/*显示管理员密码输入界面，并删除图片在链表中的路径*/
bool Administrator_Interface()
{
    //0. 显示输入密码界面
	Show_Bmp(360, 450, JOIN_PATH, 220, 0, NOTHING);	//显示输入密码界面
    //1. 创建字符串记录输入数字与密码
	char number[5];     //记录输入的数字
	bzero(number, 5);   //清零
	int x1, y1, x2, y2;
	char password[5] ="1234";	//默认密码1234
	int i = 0;  //记录光标的位置
    //2. 进行输入密码的操作
	while(1)
	{
		Get_XY(&x1, &y1, &x2, &y2); //获取每次点击屏幕的坐标
		if(i != 4)
		{
			if(x1>=340 && x1<460 && y1>=370 && y1<450)	//点击0
			{
				number[i] = '0';
				Show_Bmp(88, 74, STAR_PATH, 90*i+221, 53, NOTHING);
				i++;
			}
			else if(x1>=220 && x1<340 && y1>=290 && y1<370)	//点击1
			{
				number[i] = '1';
				Show_Bmp(88, 74, STAR_PATH, 90*i+221, 53, NOTHING);
				i++;
			}
			else if(x1>=340 && x1<460 && y1>=290 && y1<370)	//点击2
			{
				number[i] = '2';
				Show_Bmp(88, 74, STAR_PATH, 90*i+221, 53, NOTHING);
				i++;
			}
			else if(x1>=460 && x1<580 && y1>=290 && y1<370)	//点击3
			{
				number[i] = '3';
				Show_Bmp(88, 74, STAR_PATH, 90*i+221, 53, NOTHING);
				i++;
			}
			else if(x1>=220 && x1<340 && y1>=210 && y1<290)	//点击4
			{
				number[i] = '4';
				Show_Bmp(88, 74, STAR_PATH, 90*i+221, 53, NOTHING);
				i++;
			}
			else if(x1>=340 && x1<460 && y1>=210 && y1<290)	//点击5
			{
				number[i] = '5';
				Show_Bmp(88, 74, STAR_PATH, 90*i+221, 53, NOTHING);
				i++;
			}
			else if(x1>=460 && x1<580 && y1>=210 && y1<290)	//点击6
			{
				number[i] = '6';
				Show_Bmp(88, 74, STAR_PATH, 90*i+221, 53, NOTHING);
				i++;
			}
			else if(x1>=220 && x1<340 && y1>=130 && y1<210)	//点击7
			{
				number[i] = '7';
				Show_Bmp(88, 74, STAR_PATH, 90*i+221, 53, NOTHING);
				i++;
			}
			else if(x1>=340 && x1<460 && y1>=130 && y1<210)	//点击8
			{
				number[i] = '8';
				Show_Bmp(88, 74, STAR_PATH, 90*i+221, 53, NOTHING);
				i++;
			}
			else if(x1>=460 && x1<580 && y1>=130 && y1<210)	//点击9
			{
				number[i] = '9';
				Show_Bmp(88, 74, STAR_PATH, 90*i+221, 53, NOTHING);
				i++;
			}
		}
		else if(x1>=220 && x1<340 && y1>=370 && y1<450)	//确认
		{
			if(strcmp(number, password) == 0)   //如果密码输入正确删除图片路径并返回true，否则返回false
			{
				// P_Node q = BMP->Prev;
				// if(q == bmp_head)
				// {
				// 	q = q->prev;
				// }
				// Bmp_Delete(bmp_head, bmp_p->name);	//从链表中删除该图片
				// bmp_p = q;
                showBmp(88,80,TRUE_PATH);
                sleep(1);
				return true;
			}
			else
			{
				showBmp(88,80,FALSE_PATH);
                sleep(1);
                return false;
			}
		}
		if(x1>=460 && x1<580 && y1>=370 && y1<450 && i>0)	//删除，退格
		{
			i--;
			number[i] = '\0';
			Show_Bmp(88, 74, BLANK_PATH, 90*i+221, 53, NOTHING);
		}
	}
}

// int main(int argc, char const *argv[])
// {

//     int x , y ;

//     // 初始化触摸屏
//     int fd_ts = Init_Touch( );

//     while(1)
//     {
//         // 获取坐标值
//         enum DirectionOf data = Slid( fd_ts );
//         switch (data)
//         {
//         case Up:
//             printf("当前向上滑动....\n");
//             break;
//         case Down:
//             printf("当前向下滑动....\n");
//             break;
//         case Lift:
//             printf("当前向左滑动....\n");
//             break;
//         case Right:
//             printf("当前向右滑动....\n");
//             break;
        
//         default:
//             printf("未知滑动方向....\n");
//             break;
//         }
//     }


//     return 0;
// }
