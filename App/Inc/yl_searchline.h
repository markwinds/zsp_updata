#ifndef _yl_searchroad_H_
#define _yl_searchroad_H_

#include "common.h"
#include "include.h"
#include "pid.h"

enum road_condition{
    straightaway,
    bend,
    circular,
    cross,
    rest
};
extern enum road_condition road_mark;
extern int8 state_line[5];
extern int8 Ma_Mark;
extern int8 is_rightcircul_flag;
extern int8 is_leftcircul_flag;

extern int8 left_virtual[CAMERA_H];
extern int8 right_virtual[CAMERA_H];

extern float Ma_Offset;
extern float Be_Offset;
extern float La_Offset;

int8 Judge_block(int16 *jh, uint8 *s_num);
void Judge_body();
void Judge_circul();
#ifdef OLD
void Get_error_cal(float*, int*);
#else
void Get_error_cal(float*);
#endif 

void Get_middle_line();
void Goin_rightcircul();
void Goin_leftcircul();
int8 Count_black(int16 jh, int8 start, int8 end, int8 extent);

void FullLine(Site_t, Site_t, int8 line[]);
uint8 IsStraight(int8 line[]);
uint8 FullBend(int8, int8, int8);
int8 BlockOffset(int8,int8);
void CheckCircul(int8);

#endif

/*
偏差表
关于丢点判弯补线
k值随偏差变化
*/