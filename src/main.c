/**********************************************************************************************
项目名称：小区信息显示屏 / 电梯广告屏 / 餐馆菜单展示 / 广告牌 ........

基本要求：
1、开发板上电后自动运行程序
2、检索指定路径下的所有的图像文件（jpg. bmp） 
    a.存储到链表中作为播放的列表
4、开发板可以进行图像的轮播
    a.自动播放（每隔2秒）
    b.使用触摸屏进行点击播放 （滑动/点击）

拓展：
1、使用非阻塞属性设置，实现图像的 轮播可以手动也可以同时自动
    a.如果用户5秒内没有任何的操作，开发板进入自动轮播
    b.滑动/点击屏幕实现图像的轮播
2、资源更新	(FAT32格式的U盘 / 模拟一个目录)
    a.检索指定目录（U盘） 中的所有的图像文件（JPG / BMP） 并存入一个链表
    b.把该链表中的所有文件拷贝到项目中的指定目录（把U盘的文件拷贝（使用文件读写进行）到 开发板中）
3、保存播放列表/读取播放列表
    a.把播放列表写入到指定的文件中
    b.从指定文件中读取信息生成播放列表
4、文字通知显示（天气/时间等信息)
5、滑动解锁界面
**********************************************************************************************/

#include <stdio.h>
#include "MyTypes.h"
#include "dir.h"
#include "list.h"
#include <unistd.h>
#include "lcd.h"
#include "ts.h"

P_Node tmp =NULL;
P_Node BMP;

void The_Opening(void)
{
//   while (1)
//   {
//     //showBmp(100,100,cartoonhead->data.PathName) ;
//     lcd_draw_jpg(0,0,cartoonhead->data.PathName);
//     cartoonhead = cartoonhead->Prev ;
//     usleep(40000);
//     if(fopen(cartoonhead->data.PathName, "r+") == NULL)
//     {
//       return ;
//     }
//   }

    system("./mplayer -zoom -x 800 -y 480  ./intro.mp4  "); //运行程序，播放动画
    return ;
  
}

int main(int argc, char const *argv[])
{
    if(argc != 2)
    {
        printf("请按照格式执行命令：%s <图片文件夹路径名>\n", argv[0]);
        return -1;
    }

    lcd_open();
	int fd_ts = Init_Touch( );
    /* 初始化链表 */
    P_Node JPG=NewNode(NULL);
    BMP=NewNode(NULL);
    //P_Node OPENING=NewNode(NULL);

    /* 初始化目录结构体并存入链表 */
    int count_jpg = DepthTheCatalog( JPG , ".jpg"  , "."  ) ;
    int count_bmp = DepthTheCatalog( BMP, ".bmp"  , argv[1] );
    //int count_opening = DepthTheCatalog( OPENING, ".jpg"  , "./opening" );

    // 遍历显示 【列表】    
    DisplayList(JPG) ;
    DisplayList(BMP) ;
    //DisplayList(OPENING) ;
    
#if 1
    /* 开机动画 */
    The_Opening();
#endif

    /* 滑动解锁 */
    //Slide_Unlock();	

    /* 进入相册界面 */
    //Album();
    Thumbnail_Interface();

    /* 结束程序 */

    return 0;
}


