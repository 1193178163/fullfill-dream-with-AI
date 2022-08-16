#ifndef     MY_TYPES_H
#define     MY_TYPES_H

// 设计数据域
typedef struct data
{
    char PathName [257] ;
    char Type ; // J --》 JPG 文件  M --》 MP3 文件  ....
}dataType ;


// 链表节点设计
typedef struct node
{
    dataType data;

    struct node * Next;
    struct node * Prev;

}Node , *P_Node;




#endif
