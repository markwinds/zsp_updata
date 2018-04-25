#include "include.h"
#include "motor.h"
#include "common.h"

int quad_speed[3] = { 0,0,0 };
long quad_value[2] = { 0,0 };
int quad_temp = 0;
int total_distance = 0;
int cross_distance_count = 0;



/*********************************************************************************************
函数功能：更新速度和距离
步骤：


**********************************************************************************************/
void Update_Motor()
{
	int i = 0;
	int j = 0;
	int distance_temp = 0;

	/*--------------移动速度队列------------------*/
	for (i = 2; i > 0; i--)
	{
		quad_speed[i] = quad_speed[i - 1];
	}
	quad_value[1] = quad_value[0];


	/*------------获取编码器的值并更新速度--------------*/
	quad_value[0] = quad_val;                          //得到编码器的值，其值为一出发开始的累加值
	quad_speed[0] = quad_value[0] - quad_value[1];     //这次累加值减去上次的累加值就是这段时间的速度
	quad_temp += (quad_value[0] - quad_value[1]);      


	/*------------各种距离上的控制----------------*/
	while(quad_temp >= 132)//已经行驶的1cm
	{
		quad_temp -= 132;
		distance_temp++;
	}
	total_distance += distance_temp;
	if (1 == iscross_flag || 2 == iscross_flag)                    //初步判断是十字或者已经判断十字后开始累积距离
	{
		cross_distance_count += distance_temp;
	}
	if (cross_distance_count > 8 && 1 == iscross_flag)        //如果处在初判阶段且距离大于8那就清空距离判断失败      以后速度快这里应该放在中断里
	{
		iscross_flag = 0;
		cross_distance_count = 0;
	}
	if (cross_distance_count > 30 && 2 == iscross_flag)    //如果在判断是十字后跑了30厘米那就清空
	{
		iscross_flag = 0;
		cross_distance_count = 0;
	}
}