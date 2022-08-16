#include "dir.h"
#include "MyTypes.h"



int DepthTheCatalog( P_Node head , const char * SearchTypes , const char * SearchPath  ) 
{
    int count_Num = 0 ;
    // 打开指定的路径 (目录文件)
    DIR * dp = opendir( SearchPath );
    if ( dp == NULL )
    {
        fprintf( stderr ,"open dir %s error : %s \n" , SearchPath , strerror(errno));
        return 0 ;
    }
    
    

    while(1)
    {

        // 读取目录文件
        struct dirent * dirinfo = readdir( dp );
        if ( dirinfo == NULL )
        {
            fprintf( stderr ,"read dir %s error : %s \n" , SearchPath , strerror(errno));
            break ;
        }
         

        // 过滤隐藏文件  直接跳出本次循环读取下一个
        if (dirinfo->d_name[0] == '.')
        {
            continue; 
        }
         

        // 如果是目录文件则需要深度检索
        if ( dirinfo->d_type == 4 )
        {
            // 重新整合路径+目录名
            char path [257] = {0} ;
            snprintf( path , 257 , "%s/%s" , SearchPath , dirinfo->d_name );
            count_Num += DepthTheCatalog( head , SearchTypes , path  ) ;
        }
 
        // 如果不是目录也不是隐藏文件则检查是否为指定类型 文件  SearchTypes 
        // asdf.jpg.mp3
        // 找到文件名中最后一个 [.] 
        char * ptr = strrchr(dirinfo->d_name , '.' );
        if ( ptr == NULL )
        {
            continue; 
        }
        

        if ( ! strcmp( ptr , SearchTypes ) )
        {
            // 如果是 ， 则需要把当前文件的路径 + 名字 存入到链表中
            // 定义一个数据的内容
            dataType newData ;
            snprintf( newData.PathName , 257 , "%s/%s" , SearchPath , dirinfo->d_name );
            newData.Type = SearchTypes[1] ; // SearchTypes .jpg .mp3 .asd
 
            // 使用该数据内容创建一个新的链表节点
            P_Node new = NewNode( &newData );
            // 把新节点插入到链表中
            InsList( head , new , head->Next );
 
            // 计数值 ++ 
            count_Num ++ ;
        }
         
    }
    
    // 返回计数值
    return count_Num ;
}

//遍历显示图片
int Bmp_Display(P_Node head)
{
	//0.判断表头是否为空
	if(NULL == head)
	{
		printf("list is NULL!");
		return 0;
	}
	P_Node p = head->Next;
	//1.判断链表是否有数据
	if(p == head)	
	{
		printf("该文件夹没有bmp图片！\n");
		return 0;
	}
	//2.遍历链表打印数据
	int i = 0;
	printf("bmp图片有：：");
	while(p != head) 	
	{
		i++;
		printf("%s ", p->data.PathName);
		p = p->Next;
	}
	printf("\n共%d张图片\n", i);
	return i;	//返回图片数量
}