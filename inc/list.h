#ifndef     LIST_H
#define     LIST_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>



int DisplayList( P_Node head );
P_Node NewNode( dataType * NewData );

/*
 * 函数名：InsList
 * 功  能：把新节点new 添加到链表的头部位置
 * 参  数：@Prev 新节点的目标前驱节点  @new 需要插入的新节点  @Next 新节点的目标后继节点
 * 返回值： 无
*/
void InsList( P_Node Prev , P_Node new , P_Node Next );


#endif