#ifndef _searchroad_H_
#define _searchroad_H_

#include "common.h"
#include "include.h"

#define	LINE_NUM         60      //需要扫描的行数
#define BLANK_LINE_NUM   0       //开始时跳过最开始的几行
#define OFF_CENTER       5       //下一行扫描偏离上一行中心的像素个数
#define DEGREE_OF_ERROR  5       //当行与行之间的中心线差值超过此值时此数据将会被舍弃
#define CLOSE_TO_EDGE    5       //中心线接近图像的边缘，结束扫描                             这个值在中线判别更改后也要更改

#define CAMERA_HARDWARE_ERROR  1          //摄像头杆偏离中线导致的硬件偏差,右偏为正
#define TRAPEZOID_CORRECT   0.15     //梯形矫正 此值越大矫正力度越大
#define ROAD_WIDE	        39    //路宽的一半
//#define DIP_CORRECT         1.414   //倾角矫正

/*以下的宏可能以后用不到*/
#define ROAD_BEGIN       65      //当在ROAD_BEGIN与ROAD_END之间的行未扫描到黑线判断是转弯
#define ROAD_END         45
#define TURN_DEGREE      13      //
#define REFLECT_LIGHT_AREA   30  //检测反光区，从第40行开始
#define REFLECT_LIGHT_ERROR  4   //由于反光导致的像素偏差超过此值时将之后的行数据作废


extern uint8 img[][CAMERA_W];     //存放解压后图像的数组
extern uint8 imgbuff[];        //定义存储接收图像的数组
extern float average_offset[11];

extern int iscross_flag;      //判断是否是十字道的标志
extern int iscross_count;     //十字道距离计数

extern int isisland_flag;     //是否是环岛的判断 0表示没有进入
extern int isisland_flag1;
extern int isisland_count;    //环岛距离计数

extern int8 add_mark;

extern float temp_s[10];

//extern int rode_wide[CAMERA_H];




extern  void Search_line_init();
extern  void Search_line();
extern float Find_slope();
extern void Negation();
extern void nomal_middle();
extern void judge_island();
extern int8 left_black[CAMERA_H];                          //左边黑线数组
extern int8 right_black[CAMERA_H];                          //左边黑线数组
extern int8 middleline[CAMERA_H];  

#endif