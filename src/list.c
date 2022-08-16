#include "MyTypes.h"
#include "list.h"


P_Node NewNode( dataType * NewData )
{
    P_Node new = calloc(1, sizeof(Node) );

    if (NewData != NULL ) // 对数据域进行初始化
    {
        memcpy( &new->data ,  NewData , sizeof(dataType) );
    }

    // 指针域初始化
    new->Next = new->Prev = new; 
    
    return new ;
}

void InsList( P_Node Prev , P_Node new , P_Node Next )
{

    Prev->Next = new ;
    new->Prev = Prev ;

    new->Next = Next ;
    Next->Prev = new ;

    return ;
}

int DisplayList( P_Node head )
{

    if ( head == NULL || head->Next == NULL )
    {
        printf("链表头部异常或为空。\n");
        return -1 ;
    }
    

    for (P_Node tmp = head->Next ; tmp != head ; tmp = tmp->Next )
    {
        printf("当前文件路名：%s 类型：%c \n" , tmp->data.PathName , tmp->data.Type );
    }
    
    return 0 ;

}