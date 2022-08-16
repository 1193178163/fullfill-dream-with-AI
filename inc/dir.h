#ifndef     _DIR_H
#define     _DIR_H

#include <stdio.h>
#include "MyTypes.h"
#include "list.h"

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>


/*



*/
int DepthTheCatalog( P_Node head , const char * SearchTypes , const char * SearchPath  ) ;
int Bmp_Display(P_Node head);


#endif