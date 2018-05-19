#ifndef __B_P_H__
#define __B_P_H__


#include "include.h"

#define P_row 128
#define P_col 178

extern Point OP_rivise[P_row][P_col];
extern char img_P[P_row][P_col];
extern int P_T[P_row][2];    //记录P图中梯形的位置，校正和分析时只对梯形区域进行校正
extern unsigned char P_buff[P_row*P_col/8];

extern void init_P();
extern void tran_O_P();
extern void show_P();


#endif
