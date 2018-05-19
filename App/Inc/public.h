#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include "include.h"


typedef struct Point
{
    char x;
    char y;
}Point;

typedef struct Point_4
{
    int x;
    int y;
}Point_4;




extern Site_t line_site;

extern void PORTA_IRQHandler();
extern void DMA0_IRQHandler();
extern void init_Sys();
extern void do_Sys();


#endif
