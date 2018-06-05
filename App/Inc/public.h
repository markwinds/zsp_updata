#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include "include.h"


extern Site_t line_site;
extern Site_t site;
extern Size_t size;
extern Size_t imgsize;
//extern int _temp;
extern Site_t line_site;

extern float tem_st;

extern void init_Sys();
extern void do_Sys();
void DcdMode();
void Controll();
void ClearPid();
#endif