#ifndef __O_B_H__
#define __O_B_H__


#include "include.h"

#define ROW 73
#define COL 98

extern char img_bucket[ROW][COL];

extern Point OB_rivise[ROW][COL];   //桶形校正表


extern void init_B(char img_bucket[][COL]);
extern void show_B();
extern int tran_O_B(char img_origin[][80],char img_bucket[][COL]);


#endif
