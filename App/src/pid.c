/****************************************************************
 * 	
 *                          更改记录
 * <2018.5.1><yl>:        
 * 		我根据了资料修改了pid，按我的理解统哥使用的pid思路是电机的，舵机应该
 * 只有p和d来控制，而且p就是单纯的偏差值,听说还有一个基值。               
****************************************************************/

#include "pid.h"
#include "include.h"
#include "common.h"


Speed_mode car_mode = CHECH;
float steer_engine_degree = 0;              //舵机的偏转角度，可正可负，正右负左
float motor_speed = 100;
float STEER_KP = 4;
float STEER_KI = 4;
float STEER_KD = 4;
float MOTOR_KP = 3;
float MOTOR_KI = 0;
float MOTOR_KD = 0;
int32 iError = 0; //当前误差
int32 LastError = 0; //当前误差
int32 PrevError = 0;

void Control_core()
{

	if (LOW_SPEED == car_mode)//低速模式
	{
		steer_engine_degree = average_offset[1]*4.5;
		if (steer_engine_degree > DEGREE_MAX) steer_engine_degree = DEGREE_MAX;
		if (steer_engine_degree < -DEGREE_MAX) steer_engine_degree = -DEGREE_MAX;
		motor_speed = 100;
	}

	else if (CHECH == car_mode)//调试模式
	{
		Steer_Pid();
		steer_engine_degree = average_offset[0];
		if (steer_engine_degree > DEGREE_MAX) steer_engine_degree = DEGREE_MAX;
		if (steer_engine_degree < -DEGREE_MAX) steer_engine_degree = -DEGREE_MAX;
		//motor_speed = 0;
	}	

	else if (OTHER == car_mode)//其他模式
	{
		Steer_Pid();
		steer_engine_degree = average_offset[0];
		if (steer_engine_degree > DEGREE_MAX) steer_engine_degree = DEGREE_MAX;
		if (steer_engine_degree < -DEGREE_MAX) steer_engine_degree = -DEGREE_MAX;
		//if (total_distance > 1000)
		//{
		//	motor_speed = 0;
		//}
		//else
		//{
		//	motor_speed = 150;
		//}
	}
}


void Steer_Pid()
{
	float offset_p = 0;
	float offset_i = 0;
	float offset_d = 0;

	offset_p = average_offset[1];
	offset_d = average_offset[1] - average_offset[2];
	//for (i = 1; i < 6; i++)
	//{
	//	offset_d += average_offset[i];
	//}
	//for (i = 6; i < 11; i++)
	//{
	//	offset_d -= average_offset[i];
	//}
	// if (average_offset[0] < 4)
	// {
	// 	average_offset[0] = STEER_KP * offset_p + 3 * offset_i + STEER_KD * offset_d;
	// }
	// else
	// {
	// 	average_offset[0] = STEER_KP * offset_p + STEER_KI * offset_i + STEER_KD * offset_d;
	// }
	average_offset[0] = STEER_KP * offset_p + STEER_KD * offset_d;
}

//增量式PID 电机 控制
int32 PID_Realize(int32 ActualSpeed, int32 SetSpeed)
{
	//当前误差，定义为寄存器变量，只能用于整型和字符型变量，提高运算速度
	int32 Increase; //最后得出的实际增量

	iError = SetSpeed - ActualSpeed; //计算当前误差
	//加速度 ********************强制装换数据类型 防止数据出错*********************
	Increase = (int)(MOTOR_KP * (iError - LastError) + MOTOR_KI * iError + MOTOR_KD * (iError - 2 * LastError + PrevError));
	PrevError = LastError; //更新前次误差
	LastError = iError;		   //更新上次误差

	return Increase;
}