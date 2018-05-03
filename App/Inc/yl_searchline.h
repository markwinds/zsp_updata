#ifndef _yl_searchroad_H_
#define _yl_searchroad_H_

#include "common.h"
#include "include.h"

enum road_condition{
    straightaway,
    bend,
    circular,
    cross,
    rest
};

extern enum road_condition road_mark;
extern int8 state_line[5];

int8 Judge_block(int16 *jh, uint8 *s_num);
void Judge_body();
void Get_error_cal();
void Get_middle_line();

#endif