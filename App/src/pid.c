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
int speed_add = 0;
float steer_engine_degree = 0;              //舵机的偏转角度，可正可负，正右负左
float motor_speed = 0;
float STEER_KP = 4;
float STEER_KI = 4;
float STEER_KD = 4;
float MOTOR_KP = 2;
float MOTOR_KI = 2;
float MOTOR_KD = 2;

void Control_core()
{

	if (LOW_SPEED == car_mode)//低速模式
	{
		steer_engine_degree = average_offset[1]*4.5;
		//motor_Pid();		
		if (steer_engine_degree > DEGREE_MAX) steer_engine_degree = DEGREE_MAX;
		if (steer_engine_degree < -DEGREE_MAX) steer_engine_degree = -DEGREE_MAX;
		motor_speed = 100;
	}

	else if (CHECH == car_mode)//调试模式
	{
		Steer_Pid();
		//motor_Pid();
		steer_engine_degree = average_offset[0];
		//motor_speed +=speed_add;
		if (steer_engine_degree > DEGREE_MAX) steer_engine_degree = DEGREE_MAX;
		if (steer_engine_degree < -DEGREE_MAX) steer_engine_degree = -DEGREE_MAX;
		if(motor_speed<0) motor_speed=0;
	}	

	else if (OTHER == car_mode)//其他模式
	{
		Steer_Pid();
		motor_Pid();
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

void motor_Pid()
{
	speed_add=MOTOR_KP*((double)(pulse_error[0]-pulse_error[1])*0.1)+MOTOR_KI*((double)(pulse_error[0])*0.1)+MOTOR_KD*((double)(pulse_error[0]+pulse_error[2]-2*pulse_error[1])*0.1);
}